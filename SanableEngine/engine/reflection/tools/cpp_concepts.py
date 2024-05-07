from enum import Enum
import zlib
import itertools
import os
from types import NoneType
from typing import Generator
import typing
from clang.cindex import AccessSpecifier
from clang.cindex import *
from textwrap import indent
import copy
import abc
import pickle

import config
from source_discovery import SourceFile


def _getAbsName(target: Cursor) -> str:
    if type(target) == type(None) or target.kind == CursorKind.TRANSLATION_UNIT:
        # Root case: Translation unit has no name
        return ""
    else:
        # Loop case

        # Strip leading C-style record type specifier
        ownName = target.displayname
        def stripexact(val: str, leading: str): return val[len(leading):] if val.startswith(leading) else val
        ownName = stripexact(ownName, "enum ")
        ownName = stripexact(ownName, "class ") # Since this is after "enum" it will also catch "enum class"
        ownName = stripexact(ownName, "struct ")

        # Concat and loop
        return _getAbsName(target.semantic_parent) + "::" + ownName

def _isTemplate(kind: CursorKind):
    return kind in [
        CursorKind.CLASS_TEMPLATE,
        CursorKind.FUNCTION_TEMPLATE
    ]

def _typeGetAbsName(target: Type, noneOnAnonymous=True) -> str | None:
    # Special case: Can't deduce auto, decltype(auto)
    if target.spelling in ["auto", "decltype(auto)"]: return target.spelling
    
    # Special case: Can't reference an anonymous struct by name
    if noneOnAnonymous and any([(i in target.spelling) for i in ["(unnamed struct at ", "(unnamed union at ", "(unnamed class at "] ]): return None
    
    out: str

    mainDecl: Cursor = target.get_declaration()
    hasMainDecl = (len(mainDecl.spelling) != 0)

    # Resolve namespaces
    pointedToType: Type = target.get_pointee()
    if pointedToType.kind != TypeKind.INVALID:
        # Pointer case: unwrap and abs-ify pointed-to type
        out = _typeGetAbsName(pointedToType)
        cvUnwrapped = cvpUnwrapTypeName(target.spelling, unwrapPointers=False, unwrapArrays=False)
        if cvUnwrapped.endswith("&&"): out += "&&"
        elif cvUnwrapped.endswith("&"): out += "&"
        elif cvUnwrapped.endswith("*"): out += "*"
        elif cvUnwrapped.endswith("]"):
            arrayPart = cvUnwrapped[cvUnwrapped.rfind("["):]
            out = out[:-len(arrayPart)]
            if   cvUnwrapped[:-len(arrayPart)].endswith("(&&)"): out += "(&&)"
            elif cvUnwrapped[:-len(arrayPart)].endswith("(&)"): out += "(&)"
            elif cvUnwrapped[:-len(arrayPart)].endswith("(*)"): out += "(*)"
            out += arrayPart
        else:
            groupStarts = [i   for i in range(len(cvUnwrapped)) if cvUnwrapped[i]=="(" and cvUnwrapped[i  :].count(")")==cvUnwrapped[i  :].count("(")]
            groupEnds   = [i+1 for i in range(len(cvUnwrapped)) if cvUnwrapped[i]==")" and cvUnwrapped[i+1:].count(")")==cvUnwrapped[i+1:].count("(")]
            assert len(groupStarts) == len(groupEnds)
            parenGroups = [
                cvUnwrapped[ groupStarts[idx]:groupEnds[idx] ]
                for idx in range(len(groupStarts))
            ]
            if len(parenGroups) >= 2 and ">" not in cvUnwrapped[groupEnds[-2]:groupStarts[-1]]: # It's a function pointer
                assert out[-len(parenGroups[1]):] == parenGroups[1]
                out = out[:-len(parenGroups[-1])] + parenGroups[-2] + out[-len(parenGroups[-1]):] # This is so stupid.
                # TODO unwrap further
            else:
                assert False, f"Tried to unwrap {target.spelling} to {pointedToType.spelling}, but couldn't detect pointer or reference"

    elif not hasMainDecl:
        # Primitive types
        out = target.spelling

    else:
        if mainDecl.kind == CursorKind.TYPEDEF_DECL:
            # Typedefs can't be templated. Don't bother following them.
            out = mainDecl.spelling

        else:
            # Main case: Try to resolve
            if mainDecl.kind == CursorKind.TYPE_ALIAS_DECL:
                # Using statements ("using Ty = ...")
                out = mainDecl.type.get_canonical().get_declaration().spelling
            else:
                out = mainDecl.spelling

            # Recurse over template args
            templateArgs = [target.get_template_argument_type(i) for i in range(target.get_num_template_arguments())]
            templateArgStr = ", ".join([_typeGetAbsName(i) for i in templateArgs])

            if len(templateArgs) != 0:
                out = out.split("<")[0]
                out += "<"+templateArgStr+">"
    
        # Abs-ify name
        if hasMainDecl:
            out = _getAbsName(mainDecl.semantic_parent) + "::" + out
            if out.startswith("::::"): out = out[2:]

    # Resolve qualifiers
    def addQualifier(s: str, qual: str):
        baseName = s.split("<")[0] + (s.split(">")[1] if ">" in s else "")
        alreadyHasQualifier = (baseName.find("*") < baseName.find(qual))
        if not alreadyHasQualifier:
            s += " "+qual
        return s

    if target.is_const_qualified():
        out = addQualifier(out, "const")
    if target.is_volatile_qualified():
        out = addQualifier(out, "volatile")
    if target.is_restrict_qualified():
        out = addQualifier(out, "restrict")

    return out


def cvpUnwrapTypeName(name: str, unwrapCv=True, unwrapPointers=True, unwrapArrays=True) -> str:
    # Preprocess name
    name = name.strip()

    # Pointers
    if unwrapPointers:
        if name[-1] in "*&":
            return cvpUnwrapTypeName(name[:-1], unwrapCv=unwrapCv, unwrapPointers=unwrapPointers, unwrapArrays=unwrapArrays)

    # Arrays
    if unwrapArrays:
        if name.endswith("]"):
            return cvpUnwrapTypeName(name[:name.rfind("[")], unwrapCv=unwrapCv, unwrapPointers=unwrapPointers, unwrapArrays=unwrapArrays)

    # Outer const/volatile
    if unwrapCv:
        # const/volatile qualifiers (after name) - will only be present if already a ptr
        for symbolToStrip in ["const", "volatile"]:
            if name.endswith(symbolToStrip):
                charBeforeSymbol = name[-len(symbolToStrip)-1]
                if not (charBeforeSymbol.isalnum() or charBeforeSymbol == "_"):
                    return cvpUnwrapTypeName(name[:-len(symbolToStrip)], unwrapCv=unwrapCv, unwrapPointers=unwrapPointers, unwrapArrays=unwrapArrays)
            
        if name[-1] not in "*&":
            # const/volatile qualifiers (before name) - only if not already a ptr
            for symbolToStrip in ["const ", "volatile "]:
                if name.startswith(symbolToStrip):
                    return cvpUnwrapTypeName(name[len(symbolToStrip):], unwrapCv=unwrapCv, unwrapPointers=unwrapPointers, unwrapArrays=unwrapArrays)

    # Nothing to strip
    return name

class Annotations:
    DO_IMAGE_CAPTURE = "stix::do_image_capture"
    IMAGE_CAPTURE_BACKEND = "stix::image_capture_backend"
    PREFERRED_CTOR = "stix::image_capture_prefer_ctor"

    @staticmethod
    def evalAsBool(val: str):
        if val.lower() in ["y", "yes", "true" , "enable" , "enabled" , "on" ]: return True
        if val.lower() in ["n", "no" , "false", "disable", "disabled", "off"]: return False
        assert False

    @staticmethod
    def getOwn(cursor: Cursor) -> dict[str, str|None]:
        """Detect annotations passed by clang::annotate, on given cursor only"""
        annotations: dict[str, object] = dict()
        for i in cursor.get_children():
            if i.kind == CursorKind.ANNOTATE_ATTR:
                text: str = i.displayname
                if text.startswith("stix::"):
                    splitIndex = text.find("=")
                    key = text[:splitIndex].strip()
                    val = text[splitIndex+1:].strip() if splitIndex != -1 else None
                    annotations[key] = val
        return annotations

    @staticmethod
    def getAll(cursor: Cursor) -> dict[str, str|None]:
        """Detect annotations passed by clang::annotate, on given cursor and all parents"""

        annotations: dict[str, object] = Annotations.getOwn(cursor)

        # Attempt to recurse
        parent = cursor.semantic_parent
        if type(parent) != NoneType:
            inherited = Annotations.getAll(parent) # TODO we could get better performance by caching this
            for k in inherited.keys():
                if k not in annotations.keys(): annotations[k] = inherited[k]

        return annotations


class Symbol:
    __metaclass__ = abc.ABCMeta

    def __init__(this, module: "Module", cursor: Cursor):
        this.module = module
        this.astKind = cursor.kind
        this.relName = cursor.displayname
        this.absName = _getAbsName(cursor)
        this.relReferenceableName = cursor.spelling
        this.absReferenceableName = this.absName[:-len(this.relName)] + this.relReferenceableName
        this.isDefinition = cursor.is_definition()
        this.sourceFile = SourceFile(cursor.location.file.name)

        # Detect annotations passed by clang::annotate
        this.annotations = Annotations.getAll(cursor)

    
    def __repr__(this):
        return this.absName
    
    def renderPreDecls(this) -> list[str]: # Used for public_cast shenanigans
        return []

    def renderMain(this) -> str | None:
        return None

    def getReferencedTypes(this) -> list[str]:
        return []

    def hasAnnotation(this, name) -> bool:
        return name in this.annotations.keys()

    def getAnnotationOrDefault(this, name: str, default: str) -> str:
        if name in this.annotations.keys() and this.annotations[name] != None:
            return this.annotations[name]
        else:
            return default
        
    def finalize(this): pass


class Member(Symbol):
    __metaclass__ = abc.ABCMeta

    class Visibility:
        Public    = "MemberVisibility::Public"
        Protected = "MemberVisibility::Protected"
        Private   = "MemberVisibility::Private"

        def lookupFromClang(clangRepr: AccessSpecifier):
            return {
                AccessSpecifier.PUBLIC   : Member.Visibility.Public,
                AccessSpecifier.PROTECTED: Member.Visibility.Protected,
                AccessSpecifier.PRIVATE  : Member.Visibility.Private,
                AccessSpecifier.INVALID  : Member.Visibility.Public # TODO imply that it's public but only because it isn't nested?
            }[clangRepr]
    
    def __init__(this, module: "Module", cursor: Cursor, owner: "TypeInfo"):
        super().__init__(module, cursor)
        this.owner = owner
        assert owner != None, f"{this.absName} is a member, but wasn't given an owner'"
        this.visibility = Member.Visibility.lookupFromClang(cursor.access_specifier)

        pubCastKey = this.absName
        for i in range(len(pubCastKey)):
            if pubCastKey.isascii() and not (pubCastKey[i].isalnum() or pubCastKey[i] == "_"): # Special characters in ASCII range
                pubCastKey = pubCastKey[:i] + "_" + pubCastKey[i+1:] # Replace that character with an underscore
        this.pubCastKey = pubCastKey

    
class Virtualizable(Member):
    __metaclass__ = abc.ABCMeta

    def __init__(this, module: "Module", cursor: Cursor, owner: Symbol):
        super().__init__(module, cursor, owner)
        
        # Cache simple checks: search for keywords
        this.__isExplicitVirtual = cursor.is_virtual_method() or cursor.is_pure_virtual_method()
        this.__isExplicitOverride = any([i.kind == CursorKind.CXX_OVERRIDE_ATTR for i in cursor.get_children()])
    
    def getParent(this):
        # Try cached version first
        if hasattr(this, "__cachedParent"):
            return this.__cachedParent

        # Attempt to locate parent member
        this.__cachedParent = None
        for parentInfo in this.owner.immediateParents:
            parentClass = parentInfo.backingType
            if parentClass != None:
                this.__cachedParent = parentClass.getMember(this.relName, searchParents=True)
                if this.__cachedParent != None:
                    break
        return this.__cachedParent

    @property
    def isVirtual(this):
        # Try to use simple check value first
        if this.__isExplicitVirtual:
            return True

        p = this.getParent()
        if p != None:
            # Extensive check: recurse into parents
            return isinstance(p, Virtualizable) and p.isVirtual
        else:
            # Root case of extensive check: cannot recurse with no parent
            return False
    
    @property
    def isOverride(this):
        # Try to use simple check value first
        if this.__isExplicitOverride:
            return True

        # Try to recurse into parents
        p = this.getParent()
        if p != None:
            return p.isVirtual
        return False


class Callable:
    def __init__(this, module: "Module", cursor: Cursor):
        this.parameters: list[ParameterInfo] = []
        for i in cursor.get_children():
            if ParameterInfo.matches(i):
                this.parameters.append(ParameterInfo(module, i))

        this.isDeleted = cursor.is_deleted_method()


class ParameterInfo(Symbol):
    def __init__(this, module: "Module", cursor: Cursor):
        Symbol.__init__(this, module, cursor)
        this.__displayName: str = cursor.displayname
        this.__typeName = _typeGetAbsName(cursor.type)
        assert this.__typeName != None, "Cannot pass anonymous as parameter (without decltype or typedef)"

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.PARM_DECL

    @property
    def typeName(this):
        return this.__typeName

    @property
    def displayName(this):
        return this.__displayName


class GlobalFuncInfo(Symbol):
    def __init__(this, module: "Module", cursor: Cursor):
        Symbol.__init__(this, module, cursor)
        assert GlobalFuncInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a function"
        
        this.__parameters = []
        for i in cursor.get_children():
            if ParameterInfo.matches(i):
                this.__parameters.append(ParameterInfo(module, i))

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind in [
            CursorKind.CXX_METHOD,
            CursorKind.FUNCTION_DECL,
            CursorKind.FUNCTION_TEMPLATE
        ] and (cursor.is_static_method() or not TypeInfo.matches(cursor.semantic_parent))

    @property
    def parameters(this):
        return this.__parameters

    def renderMain(this):
        return None # TODO re-implement

    
class GlobalVarInfo(Symbol):
    def __init__(this, module: "Module", cursor: Cursor):
        Symbol.__init__(this, module, cursor)
        assert GlobalVarInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a variable"

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.VAR_DECL

    def renderMain(this):
        return f"//GlobalVarInfo: {this.absName}" # TODO capture address


class BoundFuncInfo(Virtualizable, Callable):
    def __init__(this, module: "Module", cursor: Cursor, owner):
        Virtualizable.__init__(this, module, cursor, owner)
        Callable.__init__(this, module, cursor)
        assert BoundFuncInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a function"
        this.returnTypeName = _typeGetAbsName(cursor.result_type)
        assert this.returnTypeName != None, "Cannot return anonymous (without decltype or typedef)"
        this.isTemplate = (cursor.kind == CursorKind.FUNCTION_TEMPLATE)
        this.isConstMethod = cursor.is_const_method()
        
        mutability = "const" if this.isConstMethod else "mut"
        this.pubCastKey += f"_{mutability}thisobj"

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind in [
            CursorKind.CXX_METHOD,
            CursorKind.FUNCTION_DECL,
            CursorKind.FUNCTION_TEMPLATE
        ] and TypeInfo.matches(cursor.semantic_parent) and not cursor.is_static_method()
    
    def renderPreDecls(this) -> list[str]: # Used for public_cast shenanigans
        if this.isTemplate or this.isDeleted: return []

        ownerName = this.owner.absName
        if ownerName.startswith("::"): ownerName = ownerName[2:]

        formatter = {
            "key": this.pubCastKey,
            "TClass": this.owner.absName,
            "returnType": this.returnTypeName,
            "params": ", ".join([i.typeName for i in this.parameters]),
            "name": this.relReferenceableName,
            "this_maybe_const": " const" if this.isConstMethod else ""
        }
        return [
            'PUBLIC_CAST_DECLARE_KEY_BARE({key});'.format_map(formatter),
		    'template<> struct ::public_cast::_type_lut<PUBLIC_CAST_KEY_OF({key})>'.format_map(formatter) + ' { ' + 'using ptr_t = {returnType} ({TClass}::*)({params}){this_maybe_const};'.format_map(formatter) + ' };',
		    'PUBLIC_CAST_GIVE_ACCESS_BARE({key}, {TClass}, {name});'.format_map(formatter)
        ]
    
    def renderMain(this):
        if not this.isDeleted:
            if not this.isTemplate:
                paramNames = [i.displayName for i in this.parameters] # TODO implement name capture
                return f"builder.addMemberFunction(stix::MemberFunction::make(DO_PUBLIC_CAST({this.pubCastKey})), \"{this.relReferenceableName}\", {this.visibility}, {str(this.isVirtual).lower()});"
            else:
                return f"//Cannot capture template function {this.absName}"
        else:
            return f"//Cannot capture deleted function {this.absName}"


class ConstructorInfo(Member, Callable):
    def __init__(this, module: "Module", cursor: Cursor, owner: "TypeInfo"):
        Member.__init__(this, module, cursor, owner)
        Callable.__init__(this, module, cursor)
        assert ConstructorInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a constructor"

        thunkTemplateArgs = ", ".join([i.typeName for i in this.parameters]) # Can't rely on template arg deduction in case of overloading
        this.absReferenceableName = f"::thunk_utils<{owner.absName}>::thunk_newInPlace<{thunkTemplateArgs}>"
        this.relReferenceableName = ""

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.CONSTRUCTOR
    
    def renderMain(this):
        if this.owner.isAbstract: return f"//Cannot capture constructor because owner is abstract: {this.absName}"
        if this.isDeleted: return f"//Cannot capture deleted constructor {this.absName}"

        if this.visibility == Member.Visibility.Public or this.owner.isFriended(lambda f: f"thunk_utils<{this.owner.absName}>" in f.targetName):
            paramNames = [i.displayName for i in this.parameters] # TODO implement name capture
            return f"builder.addConstructor(stix::StaticFunction::make(&{this.absReferenceableName}), {this.visibility});"
        else:
            return f"//Inaccessible constructor {this.absName}"
     

class DestructorInfo(Virtualizable):
    def __init__(this, module: "Module", cursor: Cursor, owner):
        Virtualizable.__init__(this, module, cursor, owner)
        assert DestructorInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a destructor"

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.DESTRUCTOR
    
    def renderMain(this):
        return None


class FieldInfo(Member):
    def __init__(this, module: "Module", cursor: Cursor, owner: "TypeInfo"):
        Member.__init__(this, module, cursor, owner)
        assert FieldInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a field"
        this.__declaredTypeName = _typeGetAbsName(cursor.type, noneOnAnonymous=False)
        this.__referenceableTypeName = _typeGetAbsName(cursor.type, noneOnAnonymous=True)
        
        this.isTypeAnonymous = (this.__referenceableTypeName == None)
        if this.isTypeAnonymous and this.visibility == Member.Visibility.Public: this.__referenceableTypeName = f"decltype({this.absReferenceableName})"

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.FIELD_DECL
    
    def finalize(this):
        if this.__referenceableTypeName != None:
            ty:"TypeInfo" = this.module.lookup(this.__referenceableTypeName)
            if ty != None and ty.visibility != Member.Visibility.Public: # FIXME handle friending
                this.__referenceableTypeName = None # If type is nested and not publically visible, mark that we couldn't reference it

    def renderPreDecls(this) -> list[str]: # Used for public_cast shenanigans
        if this.__referenceableTypeName != None:
            # These can't start with ::, otherwise we risk the lexer thinking it's one big token
            referenceableTypeName = this.__referenceableTypeName
            if referenceableTypeName.startswith("::"): referenceableTypeName = referenceableTypeName[2:]

            ownerName = this.owner.absName
            if ownerName.startswith("::"): ownerName = ownerName[2:]

            return [f'PUBLIC_CAST_GIVE_FIELD_ACCESS({this.pubCastKey}, {this.owner.absName}, {this.relName}, {referenceableTypeName});']
        else:
            return [f"#error Couldn't capture type for anonymous private field {this.absName}"]

    def renderMain(this):
        if this.__referenceableTypeName != None: # This won't be able to 
            return f'builder.addField<{this.__referenceableTypeName}>("{this.relName}", DO_PUBLIC_CAST_OFFSETOF_LAMBDA({this.pubCastKey}, {this.owner.absName}));'
        else:
            return f"#error Couldn't capture type for anonymous private field {this.absName}" # TODO attempt to break into subfields?

    def getReferencedTypes(this) -> list[str]:
        c = cvpUnwrapTypeName(this.__declaredTypeName)
        return [c]


class ParentInfo(Member):
    class Virtualness(str, Enum):
        NonVirtual       = "ParentInfo::Virtualness::NonVirtual"
        VirtualExplicit  = "ParentInfo::Virtualness::VirtualExplicit"
        VirtualInherited = "ParentInfo::Virtualness::VirtualInherited"

    def __init__(this, module: "Module", cursor: Cursor, owner: "TypeInfo"):
        Member.__init__(this, module, cursor, owner)
        assert ParentInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a parent"

        this.isVirtual = any([i.spelling == "virtual" for i in cursor.get_tokens()])
        if this.isVirtual:
            this.virtualness = ParentInfo.Virtualness.VirtualExplicit
        else:
            this.virtualness = ParentInfo.Virtualness.NonVirtual
            
    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.CXX_BASE_SPECIFIER

    @property
    def backingType(this):
        return this.module.lookup(this.absName)
    
    def renderMain(this):
        return f"builder.addParent<{this.owner.absName}, {this.absName}>({this.visibility}, {this.virtualness.value});"


class FriendInfo(Member):
    def __init__(this, module: "Module", cursor: Cursor, owner):
        Member.__init__(this, module, cursor, owner)
        this.targetName = _getAbsName([i for i in cursor.get_children()][0]) # Select friend name

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.FRIEND_DECL


class TypeInfo(Symbol):
    def __init__(this, module: "Module", cursor: Cursor):
        super().__init__(module, cursor)
        assert TypeInfo.matches(cursor), f"{cursor.kind} {cursor.displayname} is not a type"
        
        this.isAbstract = cursor.is_abstract_record()
        this.isAnonymous = cursor.is_anonymous()
        this.visibility = Member.Visibility.lookupFromClang(cursor.access_specifier)

        this.__contents: list[Member] = list()
        
        # Recurse into children
        for i in cursor.get_children():
            matchedType = next((t for t in allowedMemberSymbols if t.matches(i)), None)
            if matchedType != None:
                this.register(matchedType(module, i, this))
            elif TypeInfo.matches(i):
                module.parseGlobalCursor(i) # Nested types are effectively just namespaced. Recurse.
            elif i.kind not in ignoredSymbols:
                absName = _getAbsName(i)
                if absName != "": config.logger.warning(f"Skipping member symbol {absName} of unhandled type {i.kind}")
    
    def register(this, obj: Member):
        existing = this.getMember(obj.relName, searchParents=False)
        if existing == None or not existing.isDefinition:
            config.logger.debug(f"Registering member symbol {obj.absName} of type {obj.astKind}")
            this.__contents.append(obj)
        else:
            config.logger.warn(f"Tried to register {obj.absName} ({obj.astKind}), but a definition was already registered! New version will be discarded.")
    
    def getMember(this, relName: str, searchParents=True):
        # Scan own contents
        for i in this.__contents:
            if i.relName == relName:
                return i

        # Recurse into parents
        if searchParents:
            for pi in this.immediateParents:
                p = pi.backingType
                if p != None:
                    i = p.getMember(relName, searchParents=True)
                    if i != None:
                        return i

        # Failed to find anything
        return None
    
    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind in [
            CursorKind.CLASS_DECL, CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION,
			CursorKind.STRUCT_DECL, CursorKind.UNION_DECL
            # TODO Removed temporarily: CursorKind.CLASS_TEMPLATE
        ]
    
    @property
    def immediateParents(this) -> list[ParentInfo]:
        return [i for i in this.__contents if isinstance(i, ParentInfo)]
    
    @property
    def allParents(this) -> list[ParentInfo]:
        out = this.immediateParents
        for i in this.immediateParents:
            p = i.backingType
            if p != None: out.extend(i.backingType.allParents)
            else: config.logger.critical(f"Cannot traverse parent: {this.absName} -> {i.absName}. This may result in incorrect cast information.")
        return out
    
    @property
    def implicitlyInheritedVirtualParents(this) -> list[ParentInfo]:
        out = [copy.copy(i) for i in this.allParents if i.isVirtual and i not in this.immediateParents]

        # Create dummy redirect
        for i in out:
            i.virtualness = ParentInfo.Virtualness.VirtualInherited
            i.owner = this

        # Deduplicate by absName
        for i in out[::-1]:
            if [j.absName == i.absName for j in out].count(True) > 1:
                out.remove(i)

        return out
    
    def renderPreDecls(this) -> list[str]:
        # Skip if anonymous
        if this.isAnonymous: return []

        out = []
        for i in this.__contents:
            out.extend(i.renderPreDecls())
        return out

    def isFriended(this, selector:typing.Callable[[FriendInfo],bool]):
        return any([ (isinstance(i, FriendInfo) and selector(i)) for i in this.__contents ])

    def __renderImageCapture_cdo(this):
        # Gather valid constructors
        ctors = [i for i in this.__contents if isinstance(i, ConstructorInfo) and not i.isDeleted and Annotations.evalAsBool( this.getAnnotationOrDefault(Annotations.DO_IMAGE_CAPTURE, this.module.defaultImageCaptureStatus) )]
        hasDefaultCtor = len(ctors)==0 # TODO doesn't cover if default ctor is explicitly deleted
        isGeneratorFnFriended = this.isFriended(lambda f: "TypeBuilder" in f.targetName)
        if not isGeneratorFnFriended: ctors = [i for i in ctors if i.visibility == Member.Visibility.Public] # Ignore private ctors
        ctors.sort(key=lambda i: len(i.parameters))
        
        # Emit code
        if len(ctors) > 0 and len(ctors[0].parameters) == 0:
            return f"builder.captureClassImage_v1<{this.absName}>();"
        else:
            return f"#error {this.absName} has no accessible CDO-compatible constructor, and cannot have its image snapshotted"

    def __renderImageCapture_disassembly(this):
        # Gather valid constructors
        ctors = [i for i in this.__contents if isinstance(i, ConstructorInfo) and not i.isDeleted]
        hasDefaultCtor = len(ctors)==0 # TODO doesn't cover if default ctor is explicitly deleted
        isGeneratorFnFriended = any([ (isinstance(i, FriendInfo) and "thunk_utils" in i.targetName) for i in this.__contents ])
        if not isGeneratorFnFriended: ctors = [i for i in ctors if i.visibility == Member.Visibility.Public] # Ignore private ctors
        ctors.sort(key=lambda i: len(i.parameters))

        # Gather args for explicit template
        ctorParamArgs = None
        if len(ctors) > 0:
            ctorParamArgs = [this.absName] + [i.typeName for i in ctors[0].parameters]
        elif hasDefaultCtor:
            ctorParamArgs = [this.absName]

        # Emit code
        if ctorParamArgs != None:
            return f"builder.captureClassImage_v2<{ ', '.join(ctorParamArgs) }>();"
        else:
            return f"#error {this.absName} has no accessible Disassembly-compatible constructor, and cannot have its image snapshotted"

    def renderMain(this):
        # Skip if incomplete (suggesting the implementation doesn't belong to this TU)
        if not this.isDefinition: return f"//Skipping forward-declared type missing definition {this.absName}"
        
        # Skip if anonymous
        if this.isAnonymous: return f"//Skipping capture for anonymous type {this.absName}"
        
        # Render header
        out = f"TypeBuilder builder = TypeBuilder::create<{this.absName}>();\n"
        
        # Render standard members, explicit parents, and implicitly inherited virtual parents
        renderedContents = [i.renderMain() for i in (this.__contents + this.implicitlyInheritedVirtualParents)]
        out += "\n".join([i for i in renderedContents if i != None])
        
        # Render image capture, if configured
        if this.isAbstract:
            out += f"\n//{this.absName} is abstract. Skipping class image capture."
        elif not Annotations.evalAsBool( this.getAnnotationOrDefault(Annotations.DO_IMAGE_CAPTURE, this.module.defaultImageCaptureStatus) ):
            if Annotations.evalAsBool( this.module.defaultImageCaptureStatus ):
                out += f"\n//{this.absName} has opted out of class image capture." # Default-on: has opted out
            else:
                out += f"\n//{this.absName} hasn't opted in to class image capture." # Default-off: hasn't opted in
        else:
            backend = this.getAnnotationOrDefault(Annotations.IMAGE_CAPTURE_BACKEND, this.module.defaultImageCaptureBackend)
            if backend == "default": backend = this.module.defaultImageCaptureBackend
            if backend == "disassembly":
                out += "\n" + this.__renderImageCapture_disassembly()
            elif backend == "cdo":
                out += "\n" + this.__renderImageCapture_cdo()
            else:
                out += f"\n#error Unknown image capture backend: {backend}"

        # Finalize
        out += "\nbuilder.registerType(registry);"
        return f"//{this.absName}\n" + "{\n"+indent(out, ' '*4)+"\n}"

    def getReferencedTypes(this) -> set[str]:
        out = set()
        out.add(this.absName)
        for i in this.__contents:
            for t in i.getReferencedTypes():
                out.add(t)
        return out
    
    def finalize(this):
        for i in this.__contents: i.finalize()


class Module:
    def __init__(this, defaultImageCaptureStatus="enabled", defaultImageCaptureBackend="disassembly"):
        this.__symbols: dict[str, Symbol] = dict()
        this.__sourceFiles: set[SourceFile] = set()
        with open(__file__, "r") as thisFile:
            thisFileContent = "".join(thisFile.readlines())
            this.version = zlib.adler32(thisFileContent.encode("utf-8"))
            del thisFileContent
        this.defaultImageCaptureStatus  = defaultImageCaptureStatus
        this.defaultImageCaptureBackend = defaultImageCaptureBackend
    
    def configMatches(this, other):
        if this.version != other.version: return False
        return this.defaultImageCaptureStatus  == other.defaultImageCaptureStatus \
           and this.defaultImageCaptureBackend == other.defaultImageCaptureBackend

    def parseTU(this, sources: list[SourceFile]):
        isUpToDate = lambda file: next(filter(lambda i: i == file, this.__sourceFiles), None).contentsHash == file.contentsHash
        
        upToDate = [i for i in sources if i in this.__sourceFiles and isUpToDate(i)]
        outdated = [i for i in sources if i in this.__sourceFiles and not isUpToDate(i)]
        new = [i for i in sources if i not in this.__sourceFiles]
        removed = [i for i in this.__sourceFiles if i not in sources]

        config.logger.log(100, f"{len(upToDate)} up-to-date | {len(outdated)} outdated | {len(new)} new | {len(removed)} deleted")
        
        for source in outdated+removed:
            # Do removal
            symbolsToRemove = [i for i in this.__symbols.values() if i.sourceFile == source]
            for i in symbolsToRemove:
                del this.__symbols[i.absName]

        for source in outdated+new:
            # Do parsing
            this.__sourceFiles.add(source)
            config.logger.info(f"Parsing {source}")
            for cursor in source.parse().get_children():
                # Only capture what's in the current file
                if cursor.location.file.name.replace(os.altsep, os.sep) == source.path: this.parseGlobalCursor(cursor)
         
    def parseGlobalCursor(this, cursor: Cursor):
        # Stop early if we already have a definition for this symbol
        existingEntry = this.lookup(cursor)
        if existingEntry != None and existingEntry.isDefinition:
            return

        # Special case for namespaces: Just walk children
        if cursor.kind == CursorKind.NAMESPACE:
            for i in cursor.get_children():
                this.parseGlobalCursor(i)
            return

        # Build from factory
        matchedType = next((i for i in allowedGlobalSymbols if i.matches(cursor)), None)
        if matchedType != None:
            built: Symbol = matchedType(this, cursor)
            assert not isinstance(built, Member) # Sanity: Member registration happens in TypeInfo, except for statics
            
            if existingEntry == None or built.isDefinition or (existingEntry.sourceFile == built.sourceFile and existingEntry.sourceFile.contentsHash != built.sourceFile.contentsHash):
                this.register(built)

        elif cursor.kind not in ignoredSymbols:
            absName = _getAbsName(cursor)
            if absName != "": config.logger.warning(f"Skipping global symbol {absName} of unhandled type {cursor.kind}")

    def register(this, obj: Symbol):
        # Sanity check: No overwriting definitions. Declarations are fine to overwrite though.
        existing = this.lookup(obj.absName)
        if existing != None:
            assert not existing.isDefinition, f"Tried to register {obj.absName} ({obj.astKind}), but a definition was already registered!"

        # Do actual registration
        config.logger.debug(f"Registering global symbol {obj.absName} of type {obj.astKind}")
        this.__symbols[obj.absName] = obj

    def lookup(this, key: str | Cursor):
        if isinstance(key, Cursor):
            key = _getAbsName(key)
        return this.__symbols.get(key)

    def owns(this, obj: Symbol):
        return obj.sourceFile in this.__sourceFiles

    @property
    def types(this) -> Generator[TypeInfo, None, None]:
        for i in this.__symbols.values():
            if isinstance(i, TypeInfo):
                yield i
                
    def finalize(this):
        undefined = [i for i in this.__symbols.values() if not i.isDefinition]
        if len(undefined) > 0:
            config.logger.error(f"Detected {len(undefined)} declared global symbols missing definitions: {undefined}")
        for i in this.__symbols.values(): i.finalize()

    def renderBody(this) -> str:
        renders = [v.renderMain() for v in this.__symbols.values() if this.owns(v)]
        out = "\n\n".join([indent(v, ' '*4) for v in renders if v != None])
        return out

    def renderPreDecls(this) -> str:
        out = []
        for i in this.__symbols.values():
            if this.owns(i):
                out.extend(i.renderPreDecls())
        return "\n".join(out)

    def renderIncludes(this) -> set[str]:
        out = set()
        for i in this.types:
            if this.owns(i):
                config.logger.debug(f"{i.absName} references:")
                for typeName in i.getReferencedTypes():
                    match = this.lookup(typeName)
                    if match != None:
                        out.add(match.sourceFile.path)
                        config.logger.debug(f" - {typeName} @ {match.sourceFile.path}")
                    else:
                        config.logger.debug(f" - {typeName} @ (Failed to locate definition)")
        return out
            
    def save(this, cachePath: str):
        cacheDir = cachePath[:-len(os.path.basename(cachePath))]
        os.makedirs(cacheDir, exist_ok=True)
        with open(cachePath, "wb") as file:
            file.write(pickle.dumps(this))

    def load(cachePath: str) -> "Module":
        if not os.path.exists(cachePath): return Module()
        
        with open(cachePath, "rb") as file: cacheFileRepr = file.read()
        
        try: out = pickle.loads(cacheFileRepr)
        except EOFError: pass # Only happens when we have a blank file. Mundane, safe to ignore.
        
        config.logger.info(f"Loaded {len(out.__symbols)} symbols from cache")

        return out

        
ignoredSymbols = [
    # Actually ignored for good reasons
    CursorKind.CXX_ACCESS_SPEC_DECL,
    CursorKind.UNEXPOSED_DECL,
    CursorKind.UNEXPOSED_EXPR,
    CursorKind.UNEXPOSED_ATTR,
    CursorKind.STATIC_ASSERT,
    CursorKind.ALIGNED_ATTR,
    CursorKind.NAMESPACE_REF, # No point in implementing namespace aliasing

    # ??? I have no idea what these are
    CursorKind.CXX_BOOL_LITERAL_EXPR,
    CursorKind.INTEGER_LITERAL,
    CursorKind.DECL_REF_EXPR,
    CursorKind.CALL_EXPR, # Appears to be related to decltype/declval in template parameters?
    CursorKind.DECL_REF_EXPR,
    
    # Not sure if I care yet
    CursorKind.LINKAGE_SPEC,

    # TODO reimplement global variable support
    CursorKind.VAR_DECL,

    # TODO reimplement function support
    CursorKind.CONVERSION_FUNCTION,
    CursorKind.CXX_METHOD,

    # TODO implement type alias support
    CursorKind.TYPEDEF_DECL,
    CursorKind.USING_DECLARATION,
    CursorKind.USING_DIRECTIVE,
    CursorKind.TYPE_ALIAS_DECL,

    # TODO implement enum support
    CursorKind.ENUM_DECL,

    # TODO implement template support
    CursorKind.CLASS_TEMPLATE,
    CursorKind.TYPE_REF,
    CursorKind.TEMPLATE_REF,
    CursorKind.TEMPLATE_TYPE_PARAMETER,
    CursorKind.TEMPLATE_NON_TYPE_PARAMETER,
    CursorKind.TYPE_ALIAS_TEMPLATE_DECL,
    CursorKind.FUNCTION_TEMPLATE,
    CursorKind.PACK_EXPANSION_EXPR
]
allowedMemberSymbols = [FieldInfo, ParentInfo, ConstructorInfo, DestructorInfo, BoundFuncInfo, FriendInfo]
allowedGlobalSymbols = [TypeInfo] # GlobalVarInfo, GlobalFuncInfo
