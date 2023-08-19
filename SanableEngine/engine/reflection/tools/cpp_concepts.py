from types import NoneType
from typing import Generator, Iterator
from clang.cindex import *
from textwrap import indent
import abc
import hashlib
import config


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
    for symbolToStrip in ["*", "const", "volatile"]:
        if name.startswith(symbolToStrip):
            return cvpUnwrapTypeName(name[len(symbolToStrip):])
        elif name.endswith(symbolToStrip):
            return cvpUnwrapTypeName(name[:-len(symbolToStrip)])

    # Nothing to strip
    return name

class Symbol:
    __metaclass__ = abc.ABCMeta

    def __init__(this, cursor: Cursor):
        this.__astRepr = cursor
    
    @property
    def astRepr(this):
        return this.__astRepr

    @property
    def astKind(this):
        return this.__astRepr.kind
        
    @property
    def absName(this) -> str:
        if not hasattr(this, "__cachedAbsName"):
            this.__cachedAbsName = _getAbsName(this.__astRepr)
        return this.__cachedAbsName

    @property
    def relName(this) -> str:
        return this.__astRepr.displayname

    @property
    def rttiHashedName(this) -> str:
        if not hasattr(this, "__cachedRttiHashedName"):
            this.__cachedRttiHashedName = "__generatedRTTI_"+hashlib.sha256(this.absName.encode("utf-8")).hexdigest()[:8]
        return this.__cachedRttiHashedName

    def render(this) -> str | None:
        return None

    def getReferencedTypes(this) -> list[str]:
        return []


class Member(Symbol):
    __metaclass__ = abc.ABCMeta

    def __init__(this, cursor: Cursor, owner):
        Symbol.__init__(this, cursor)
        this.__owner = owner
        assert this.__owner != None, f"{this.absName} is a member, but wasn't given an owner'"
    
    @property
    def owner(this):
        return this.__owner

    
class Virtualizable(Member):
    def __init__(this, cursor: Cursor, owner):
        Member.__init__(this, cursor, owner)
        this.__cursor = cursor
        
        # Cache simple checks: search for keywords
        this.__isExplicitVirtual = cursor.is_virtual_method() or cursor.is_pure_virtual_method()
        this.__isExplicitOverride = any([i.kind == CursorKind.CXX_OVERRIDE_ATTR for i in cursor.get_children()])
    
    def getParent(this):
        # Try cached version first
        if hasattr(this, "__cachedParent"):
            return this.__cachedParent

        # Attempt to locate parent member
        this.__cachedParent = None
        for parentInfo in this.owner.parents:
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
    def __init__(this, module, cursor: Cursor):
        Symbol.__init__(this, cursor)
        this.__module = module
        this.__typeName = cursor.displayname
        #this.__name = TODO scan file

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.PARM_DECL


class FuncInfo(Virtualizable):
    def __init__(this, module, cursor: Cursor, owner):
        Virtualizable.__init__(this, cursor, owner)
        assert FuncInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a function"
        
        this.__parameters = []
        for i in cursor.get_children():
            if ParameterInfo.matches(i):
                this.__parameters.append(ParameterInfo(module, i))

        # TODO capture address

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind in [
            CursorKind.CXX_METHOD,
            CursorKind.FUNCTION_DECL,
            CursorKind.FUNCTION_TEMPLATE
        ]

    @property
    def parameters(this):
        return this.__parameters

    def renderMain(this):
        return None


class ConstructorInfo(Member):
    def __init__(this, module, cursor: Cursor, owner):
        Member.__init__(this, cursor, owner)
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
    def __init__(this, module, cursor: Cursor, owner):
        Virtualizable.__init__(this, cursor, owner)
        assert DestructorInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a destructor"

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.DESTRUCTOR
    
    def renderMain(this):
        return None


class VarInfo(Symbol):
    def __init__(this, module, cursor: Cursor):
        Symbol.__init__(this, cursor)
        assert VarInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a variable"

        # TODO deduce address (global or relative)

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.VAR_DECL

    def renderMain(this):
        return "//VarInfo "+this.rttiHashedName+" = VarInfo(); // "+this.absName # TODO re-implement


class FieldInfo(Member):
    def __init__(this, module, cursor: Cursor, owner):
        Member.__init__(this, cursor, owner)
        assert FieldInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a field"
        this.__declaredTypeName = cursor.type.spelling

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.FIELD_DECL

    def renderMain(this):
        return f'builder.addField<{this.__declaredTypeName}>("{this.relName}", offsetof({this.owner.absName}, {this.relName}));'

    def getReferencedTypes(this) -> list[str]:
        c = cvpUnwrapTypeName(this.__declaredTypeName)
        return [c]


class ParentInfo(Member):
    def __init__(this, module, cursor: Cursor, owner):
        Member.__init__(this, cursor, owner)
        this.__module = module
        assert ParentInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a parent"

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.CXX_BASE_SPECIFIER

    @property
    def backingType(this):
        return this.__module.lookup(this.absName)
    
    def renderMain(this):
        return None # Renders as part of first line of type


class TypeInfo(Symbol):
    def __init__(this, module, cursor: Cursor):
        super().__init__(cursor)
        this.__module = module
        assert TypeInfo.matches(cursor), f"{cursor.kind} {cursor.displayname} is not a type"
        
        this.__contents: list[Member] = list()
        this.__sourceFile = cursor.location.file.name
        
        # Recurse into children
        for i in cursor.get_children():
            matchedType = next((t for t in allowedMemberSymbols if t.matches(i)), None)
            if matchedType != None:
                this.register(matchedType(module, i, this))
            elif TypeInfo.matches(i):
                module.parse(i) # Nested types are effectively just namespaced. Recurse.
            elif i.kind not in ignoredSymbols:
                config.logger.warning(f"Skipping member symbol {_getAbsName(i)} of unhandled type {i.kind}")
    
    def register(this, obj):
        assert not any([obj.absName == i.absName for i in this.__contents]), f"Tried to register {obj.absName} ({obj.astKind}), but it was already registered!"
        config.logger.debug(f"Registering member symbol {obj.absName} of type {obj.astKind}")
        this.__contents.append(obj)
    
    def getMember(this, relName: str, searchParents=True):
        # Scan own contents
        for i in this.__contents:
            if i.relName == relName:
                return i

        # Recurse into parents
        if searchParents:
            for pi in this.parents:
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
            CursorKind.CLASS_DECL, CursorKind.CLASS_TEMPLATE, CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION,
			CursorKind.STRUCT_DECL, CursorKind.UNION_DECL
        ]

    @property
    def sourceFile(this) -> str:
        return this.__sourceFile

    @property
    def parents(this) -> list[ParentInfo]:
        return [i for i in this.__contents if isinstance(i, ParentInfo)]

    def renderMain(this):
        # Render header
        out = f"TypeBuilder builder = TypeBuilder::create<{this.absName}>();\n"

        # Render parents
        for i in this.parents:
            out += f"builder.addParent<{this.absName}, {i.absName}>();\n"

        # Render standard members
        renderedContents = [i.renderMain() for i in this.__contents if not isinstance(i, ParentInfo)]
        out += "\n".join([i for i in renderedContents if i != None])
        
        # Finalize
        out += f"\nbuilder.captureCDO<{this.absName}>();"
        out += "\nbuilder.registerType(registry);"
        return f"//{this.relName}\n" + "{\n"+indent(out, ' '*4)+"\n}"

    def getReferencedTypes(this) -> set[str]:
        out = set()
        out.add(this.absName)
        for i in this.__contents:
            for t in i.getReferencedTypes():
                out.add(t)
        return out


class Module:
    def __init__(this):
        this.__contents: dict[str, Symbol] = dict()
        this.roots: list[Cursor] = list()
        this.typeCursorCache: dict[str, Cursor] = dict()
        
    def registerExternal(this, cursor: Cursor):
        this.roots.append(cursor)

    def parse(this, cursor: Cursor): 
        this.roots.append(cursor)
        matchedType = next((i for i in allowedGlobalSymbols if i.matches(cursor)), None)
        if matchedType != None:
            v = matchedType(this, cursor)

            if not isinstance(v, Member) or v.isGlobal:
                this.register(v) # Global or static
            else:
                v.owner.register(v) # Members

        elif matchedType not in ignoredSymbols:
            config.logger.warning(f"Skipping global symbol {_getAbsName(cursor)} of unhandled type {cursor.kind}")

    def register(this, obj: Symbol):
        assert obj.absName not in this.__contents.keys(), f"Tried to register {obj.absName} ({obj.astKind}), but it was already registered!"
        config.logger.debug(f"Registering global symbol {obj.absName} of type {obj.astKind}")
        this.__contents[obj.absName] = obj

    def lookup(this, key: str | Cursor):
        if isinstance(key, Cursor):
            key = _getAbsName(key)
        return this.__contents.get(key)

    @property
    def types(this) -> Generator[TypeInfo, None, None]:
        for i in this.__contents.values():
            if isinstance(i, TypeInfo):
                yield i

    @property
    def globals(this) -> Generator[VarInfo, None, None]: # NOTE: Includes statics inside classes
        for i in this.__contents.values():
            if isinstance(i, VarInfo):
                yield i

    @property
    def functions(this) -> Generator[FuncInfo, None, None]:
        for i in this.__contents.values():
            if isinstance(i, FuncInfo):
                yield i

    @property
    def rttiHashedName(this) -> str:
        if not hasattr(this, "__cachedRttiHashedName"):
            this.__cachedRttiHashedName = "__generatedRTTI_"+hashlib.sha256(','.join([i.rttiHashedName for i in this.__contents.values()]).encode("utf-8")).hexdigest()[:8]
        return this.__cachedRttiHashedName

    def renderBody(this) -> str:
        out = "\n\n".join([indent(v.renderMain(), ' '*4) for v in this.__contents.values()])
        return out

    def renderIncludes(this) -> set[str]:
        out = set()
        for i in this.types:
            config.logger.debug(f"{i.absName} references:")
            for typeName in i.getReferencedTypes():
                typeCursor = locateTypeCursor(this, typeName)
                if not isinstance(typeCursor, NoneType):
                    out.add(typeCursor.canonical.location.file.name)
                    config.logger.debug(f" - {typeName} @ {typeCursor.canonical.location.file.name}:{typeCursor.canonical.location.line}")
                else:
                    config.logger.debug(f" - {typeName} @ (external)")
                    #config.logger.warn(f"Failed to locate definition of {typeName}")
        return out

def locateTypeCursor(target: Module | Cursor, name: str) -> Cursor | None:
    candidates = locateTypeCursor_impl(target, name)
    return candidates[0] if len(candidates)>0 else None

def locateTypeCursor_impl(target: Module | Cursor, name: str) -> list[Cursor]:
    out = []

    # If module, visit child cursors
    if isinstance(target, Module):

        #Try cache first
        if name in target.typeCursorCache:
            out.append(target.typeCursorCache[name])
        else:
            for i in target.roots:
                rv = locateTypeCursor(i, name)
                if not isinstance(rv, NoneType): # Can't do direct comparison to None...
                    target.typeCursorCache[name] = rv
                    out.append(rv)
    
    # If cursor, try to match, then try to recurse
    else:
        if TypeInfo.matches(target) and _getAbsName(target) == name:
            out.append(target)

        elif target.kind == CursorKind.NAMESPACE or TypeInfo.matches(target):
            for i in target.get_children():
                rv = locateTypeCursor(i, name)
                if not isinstance(rv, NoneType): # Can't do direct comparison to None...
                    out.append(rv)

    return out
    

ignoredSymbols = [CursorKind.CXX_ACCESS_SPEC_DECL]
allowedMemberSymbols = [FieldInfo, ParentInfo, ConstructorInfo, DestructorInfo, FuncInfo]
allowedGlobalSymbols = [VarInfo, FuncInfo, TypeInfo]
