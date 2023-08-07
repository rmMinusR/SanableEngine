﻿from types import NoneType
from typing import Generator, Iterator
from clang.cindex import *
from textwrap import indent
import abc
import hashlib
from config import logger


def _getAbsName(target: Cursor) -> str:
	if type(target.semantic_parent) == type(None) or target.semantic_parent.kind == CursorKind.TRANSLATION_UNIT:
		# Root case: Drop translation unit name
		return target.displayname
	else:
		# Loop case
		return _getAbsName(target.semantic_parent) + "::" + target.displayname


class Symbol:
    __metaclass__ = abc.ABCMeta

    def __init__(this, cursor: Cursor):
        this.__astRepr = cursor

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


class Ownable:
    __metaclass__ = abc.ABCMeta

    def __init__(this, module, cursor: Cursor, owner):
        this.__owner = owner

    @property
    def isGlobal(this):
        return this.__owner == None

    @property
    def isMember(this):
        return this.__owner != None

    @property
    def owner(this):
        assert this.isMember, f"{this.absName} has no owner"
        return this.__owner


class FuncInfo(Symbol, Ownable):
    def __init__(this, module, cursor: Cursor, owner=None):
        Symbol.__init__(this, cursor)
        Ownable.__init__(this, module, cursor, owner)
        assert FuncInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a function"
        
        this.__isVirtual = cursor.is_virtual_method() or cursor.is_pure_virtual_method()
        this.__isOverride = False # Obviously wrong, TODO

        # TODO capture address

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind in [
            CursorKind.CXX_METHOD,
            CursorKind.FUNCTION_DECL,
            CursorKind.FUNCTION_TEMPLATE
        ]

    @property
    def isVirtual(this):
        return this.__isVirtual

    @property
    def isOverride(this):
        return this.__isOverride

    def renderMain(this):
        return "//FuncInfo "+this.rttiHashedName+" = FuncInfo(); // "+this.absName # TODO re-implement


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


class FieldInfo(Symbol, Ownable):
    def __init__(this, module, cursor: Cursor, owner):
        Symbol.__init__(this, cursor)
        Ownable.__init__(this, module, cursor, owner)
        assert FieldInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a field"
        this.__declaredType = cursor.type.spelling

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.FIELD_DECL

    def renderMain(this):
        return f'builder.addField(TypeName::parse("{this.__declaredType}"), "{this.relName}");'


class ParentInfo(Symbol, Ownable):
    def __init__(this, module, cursor: Cursor, owner):
        Symbol.__init__(this, cursor)
        Ownable.__init__(this, module, cursor, owner)
        assert ParentInfo.matches(cursor), f"{cursor.kind} {this.absName} is not a variable"

    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind == CursorKind.CXX_BASE_SPECIFIER
    
    def renderMain(this):
        return f'builder.addParent(TypeName::parse("{this.absName}"));'


class TypeInfo(Symbol):
    def __init__(this, module, cursor: Cursor):
        super().__init__(cursor)
        assert TypeInfo.matches(cursor), f"{cursor.kind} {cursor.displayname} is not a type"

        this.__contents = list()
        this.__hasNewVtable = False
        this.__sourceFile = cursor.location.file.name
        
        #Recurse into children
        for i in cursor.get_children():
            if FieldInfo.matches(i):
                this.register(FieldInfo(module, i, this))
            elif FuncInfo.matches(i):
                info = FuncInfo(module, i)
                this.register(info)
                #Check if a new vtable would be generated for self
                if info.isVirtual and not info.isOverride:
                    this.__hasNewVtable = True
            elif TypeInfo.matches(i):
                module.parse(i) # Nested types are effectively just namespaced. Recurse.
            elif ParentInfo.matches(i):
                this.register(ParentInfo(module, i, this))
            elif i.kind not in ignoredSymbols:
                logger.warning(f"Skipping member symbol {_getAbsName(i)} of unhandled type {i.kind}")
    
    def register(this, obj):
        assert not any([obj.absName == i.absName for i in this.__contents]), f"Tried to register {obj.absName} ({obj.astKind}), but it was already registered!"
        logger.debug(f"Registering member symbol {obj.absName} of type {obj.astKind}")
        # TODO if virtual method and not overriding, set __hasVtable = True
        this.__contents.append(obj)
    
    @staticmethod
    def matches(cursor: Cursor):
        return cursor.kind in [
            CursorKind.CLASS_DECL, CursorKind.CLASS_TEMPLATE, CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION,
			CursorKind.STRUCT_DECL, CursorKind.UNION_DECL
        ]

    @property
    def sourceFile(this) -> str:
        return this.__sourceFile

    def renderMain(this):
        out = f"TypeBuilder builder = TypeBuilder::fromCDO<{this.absName}>({str(this.__hasNewVtable).lower()});\n"
        renderedContents = [i.renderMain() for i in this.__contents]
        out += "\n".join([i for i in renderedContents if i != None])
        out += "\nbuilder.registerType(registry);"
        return f"//{this.relName}\n" + "{\n"+indent(out, ' '*4)+"\n}"


class Module:
    def __init__(this):
        this.__contents = dict()

    def parse(this, cursor: Cursor): 
        matchedType = next((i for i in allowedGlobalSymbols if i.matches(cursor)), None)
        if matchedType != None:
            v = matchedType(this, cursor)

            if not isinstance(v, Ownable) or v.isGlobal:
                this.register(v) # Global or static
            else:
                v.owner.register(v) # Members

        elif matchedType not in ignoredSymbols:
            logger.warning(f"Skipping global symbol {_getAbsName(cursor)} of unhandled type {cursor.kind}")

    def register(this, obj):
        assert obj.absName not in this.__contents.keys(), f"Tried to register {obj.absName} ({obj.astKind}), but it was already registered!"
        logger.debug(f"Registering global symbol {obj.absName} of type {obj.astKind}")
        this.__contents[obj.absName] = obj

    def lookup(this, key: str | Cursor):
        if isinstance(key, Cursor):
            key = _getAbsName(key)
        return this.__contents[key]

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

    def renderIncludes(this) -> list[str]:
        return [i.sourceFile for i in this.types]


ignoredSymbols = [CursorKind.CXX_ACCESS_SPEC_DECL]
allowedGlobalSymbols = [VarInfo, FuncInfo, TypeInfo]
