from enum import Enum
import itertools
import os
from typing import Generator
from clang.cindex import AccessSpecifier
from clang.cindex import *
from textwrap import indent
import copy
import abc
import pickle

import config
from source_discovery import SourceFile


def _getAbsName(target: Cursor) -> str:
	if type(target.semantic_parent) == type(None) or target.semantic_parent.kind == CursorKind.TRANSLATION_UNIT:
		# Root case: Drop translation unit name
		return target.displayname
	else:
		# Loop case
		return _getAbsName(target.semantic_parent) + "::" + target.displayname

def cvpUnwrapTypeName(name: str) -> str:
    # Preprocess name
    name = name.strip()
    for symbolToStrip in ["*", "const", "volatile"]: # FIXME this will also catch typenames starting or ending with const or volatile
        if name.startswith(symbolToStrip):
            return cvpUnwrapTypeName(name[len(symbolToStrip):])
        elif name.endswith(symbolToStrip):
            return cvpUnwrapTypeName(name[:-len(symbolToStrip)])

    # Nothing to strip
    return name



class Symbol:
    __metaclass__ = abc.ABCMeta

    def __init__(this, module: "Module", cursor: Cursor):
        this.module = module
        this.astKind = cursor.kind
        this.relName = cursor.displayname
        this.absName = _getAbsName(cursor)
        this.isDefinition = cursor.is_definition()
        this.sourceFile = SourceFile(cursor.location.file.name)
    
    def __repr__(this):
        return this.absName
    
    def renderPreDecls(this) -> list[str]: # Used for public_cast shenanigans
        return []

    def renderMain(this) -> str | None:
        return None

    def getReferencedTypes(this) -> list[str]:
        return []


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
                AccessSpecifier.PRIVATE  : Member.Visibility.Private
            }[clangRepr]
    
    def __init__(this, module: "Module", cursor: Cursor, owner: Symbol):
        super().__init__(module, cursor)
        this.owner = owner
        assert owner != None, f"{this.absName} is a member, but wasn't given an owner'"

    
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


class ParameterInfo(Symbol):
    def __init__(this, module: "Module", cursor: Cursor):
        Symbol.__init__(this, module, cursor)
        this.__typeName = cursor.displayname
        #this.__name = TODO scan file

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.PARM_DECL


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


class BoundFuncInfo(Virtualizable):
    def __init__(this, module: "Module", cursor: Cursor, owner):
        Virtualizable.__init__(this, module, cursor, owner)
        assert BoundFuncInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a function"
        
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
        ] and TypeInfo.matches(cursor.semantic_parent) and not cursor.is_static_method()

    @property
    def parameters(this):
        return this.__parameters

    def renderMain(this):
        return f"//BoundFuncInfo: {this.absName}" # TODO capture address


class ConstructorInfo(Member):
    def __init__(this, module: "Module", cursor: Cursor, owner):
        Member.__init__(this, module, cursor, owner)
        assert ConstructorInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a constructor"
        
        this.__parameters = []
        for i in cursor.get_children():
            if ParameterInfo.matches(i):
                this.__parameters.append(ParameterInfo(module, i))

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.CONSTRUCTOR

    @property
    def parameters(this):
        return this.__parameters
    
    def renderMain(this):
        return None


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
    def __init__(this, module: "Module", cursor: Cursor, owner):
        Member.__init__(this, module, cursor, owner)
        assert FieldInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a field"
        this.__declaredTypeName = cursor.type.spelling

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.FIELD_DECL
    
    def renderPreDecls(this) -> list[str]: # Used for public_cast shenanigans
        return [f'PUBLIC_CAST_GIVE_ACCESS({this.owner.absName}, {this.relName}, {this.__declaredTypeName} {this.owner.absName}::*);']

    def renderMain(this):
        # f'builder.addField<{this.__declaredTypeName}>("{this.relName}", offsetof({this.owner.absName}, {this.relName}));'
        # f'builder.addField<decltype({this.owner.absName}::{this.relName})>("{this.relName}", offsetof({this.owner.absName}, {this.relName}));'
        return f'builder.addField<{this.__declaredTypeName}>("{this.relName}", DO_PUBLIC_CAST_OFFSETOF({this.owner.absName}, {this.relName}));'

    def getReferencedTypes(this) -> list[str]:
        c = cvpUnwrapTypeName(this.__declaredTypeName)
        return [c]


class ParentInfo(Member):
    class Virtualness(str, Enum):
        NonVirtual       = "ParentInfo::Virtualness::NonVirtual"
        VirtualExplicit  = "ParentInfo::Virtualness::VirtualExplicit"
        VirtualInherited = "ParentInfo::Virtualness::VirtualInherited"

    def __init__(this, module: "Module", cursor: Cursor, owner):
        Member.__init__(this, module, cursor, owner)
        this.dynOwner = owner
        assert ParentInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a parent"
        
        this.visibility = Member.Visibility.lookupFromClang(cursor.access_specifier)

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
        return f"builder.addParent<{this.dynOwner.absName}, {this.absName}>({this.visibility}, {this.virtualness});"


class TypeInfo(Symbol):
    def __init__(this, module: "Module", cursor: Cursor):
        super().__init__(module, cursor)
        assert TypeInfo.matches(cursor), f"{cursor.kind} {cursor.displayname} is not a type"
        
        this.isAbstract = cursor.is_abstract_record()

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
            i.dynOwner = this

        # Deduplicate by absName
        for i in out[::-1]:
            if [j.absName == i.absName for j in out].count(True) > 1:
                out.remove(i)

        return out
    
    def renderPreDecls(this) -> list[str]:
        out = []
        for i in this.__contents:
            out.extend(i.renderPreDecls())
        return out

    def renderMain(this):
        # Render header
        out = f"TypeBuilder builder = TypeBuilder::create<{this.absName}>();\n"
        
        # Render standard members, explicit parents, and implicitly inherited virtual parents
        renderedContents = [i.renderMain() for i in (this.__contents + this.implicitlyInheritedVirtualParents)]
        out += "\n".join([i for i in renderedContents if i != None])
        
        # Finalize
        if not this.isAbstract:
           out += f"\nbuilder.captureCDO<{this.absName}>();"
        else:
            out += f"\n//{this.absName} is abstract. Skipping CDO capture."
        out += "\nbuilder.registerType(registry);"
        return f"//{this.absName}\n" + "{\n"+indent(out, ' '*4)+"\n}"

    def getReferencedTypes(this) -> set[str]:
        out = set()
        out.add(this.absName)
        for i in this.__contents:
            for t in i.getReferencedTypes():
                out.add(t)
        return out


class Module:
    def __init__(this):
        this.__symbols: dict[str, Symbol] = dict()
        this.__sourceFiles: set[SourceFile] = set()
    
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
            for cursor in source.parse().get_children():
                this.parseGlobalCursor(cursor)
         
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
    CursorKind.FRIEND_DECL, # Note: Might want to implement friend checking before doing codegen, but not now

    # ??? I have no idea what these are
    CursorKind.CXX_BOOL_LITERAL_EXPR,
    CursorKind.INTEGER_LITERAL,
    CursorKind.DECL_REF_EXPR,
    CursorKind.CALL_EXPR, # Appears to be related to decltype/declval in template parameters?
    CursorKind.DECL_REF_EXPR,

    # TODO reimplement global variable support
    CursorKind.VAR_DECL,

    # TODO reimplement function support
    CursorKind.FUNCTION_DECL,
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
allowedMemberSymbols = [FieldInfo, ParentInfo, ConstructorInfo, DestructorInfo, BoundFuncInfo]
allowedGlobalSymbols = [TypeInfo] # GlobalVarInfo, GlobalFuncInfo
