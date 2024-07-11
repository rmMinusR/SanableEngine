import abc
from enum import Enum
from functools import cached_property
from source_discovery import SourceFile

class SourceLocation:
    def __init__(this, file: SourceFile, line:int, column:int):
        this.file = file
        this.line = line
        this.column = column


class ASTNode:
    __metaclass__ = abc.ABCMeta

    def __init__(this, ownerName:str|None, ownName:str|None, location: SourceLocation):
        this.ownerName = ownerName
        this.ownName = ownName
        this.location = location
        this.astParent:ASTNode|None = None
        this.children:list|None = None

    def __str__(this):
        return f"{this.path} ({type(this)})"
    
    @cached_property
    def path(this):
        return f"{this.ownerName}::{this.ownName}"
    
    @staticmethod
    def allowMultiple():
        return False

    def link(this, module: "Module"):
        if this.ownerName != None:
            this.astParent = module.find(this.ownerName)
            if this.astParent.children == None: this.astParent.children = []
            this.astParent.children.append(this)
            
    def latelink(this, module: "Module"):
        pass


class Module:
    def __init__(this):
        this.contents:dict[str, ASTNode|list[ASTNode]] = dict()
        this.byType:dict[type, list[ASTNode]] = dict()
        this.__linked = False
        
    def __getstate__(this):
        out = [i for i in this.contents.values()]
        out.sort(lambda node: node.path)
        return out
    
    def __setstate__(this, vals):
        this.contents = dict()
        this.byType = dict()
        for i in vals:
            this.contents[i.path] = i
            if type(i) not in this.byType.keys(): this.byType[type(i)] = []
            this.byType[type(i)].append(i)

    def register(this, node:ASTNode):
        assert not this.__linked, "Cannot register new nodes after linking"
        
        # Place in fast-lookup contents tree
        if not node.allowMultiple():
            assert node.path not in this.contents.keys(), f"Tried to register f{node} twice!"
            this.contents[node.path]
        else:
            if not node.path in this.contents.keys(): this.contents[node.path] = []
            this.contents[node.path].append(node)
        
        # Place in by-type lookup
        if not type(node) in this.byType.keys(): this.byType[type(node)] = []
        this.byType[type(node)].append(node)
            
    def linkAll(this):
        assert not this.__linked, "Can only link once"
        this.__linked = True
        for v in this.contents.values(): v.link()
        for v in this.contents.values(): v.latelink()
        
    def find(this, path:str) -> ASTNode:
        assert this.__linked, "Can only be called after or during linking"
        try: return this.contents[path]
        except KeyError: return None


class Annotation(ASTNode):
    def __init__(this, ownerName:str, text:str, location: SourceLocation):
        super().__init__(this, ownerName, None, location)
        this.text = text
        
    @staticmethod
    def allowMultiple():
        return True
    

class TypeInfo(ASTNode):
    def __init__(this, ownerName:str|None, ownName:str, location: SourceLocation):
        super().__init__(this, ownerName, ownName, location)
        
    @property
    def immediateParents(this):
        return (i for i in this.children if isinstance(i, ParentInfo))
     
        
class Member(ASTNode):
    
    class Visibility(str, Enum):
        Public    = "MemberVisibility::Public"
        Protected = "MemberVisibility::Protected"
        Private   = "MemberVisibility::Private"

    def __init__(this, ownerName:str, ownName:str, location:SourceLocation, visibility:Visibility):
        super().__init__(this, ownerName, ownName, location)
        this.visibility = visibility
    

class MaybeVirtual(Member):
    __metaclass__ = abc.ABCMeta
    
    def __init__(this, ownerName:str, inheritedFromName:str|None, ownName:str, location: SourceLocation, visibility:Member.Visibility, isExplicitVirtual:bool, isExplicitOverride:bool):
        super().__init__(this, ownerName, ownName, location, visibility)
        this.__isExplicitVirtual = isExplicitVirtual
        this.__isExplicitOverride = isExplicitOverride
        this.inheritedFromName = inheritedFromName # TODO hacky, search parents instead?
        this.inheritedVersion = None
        this.isVirtual = None
        this.isOverride = None
        
    def latelink(this, module: Module):
        def __isVirtual (v:MaybeVirtual): return v.__isExplicitVirtual  or __isVirtual (v.inheritedVersion) if v.inheritedVersion != None else False
        def __isOverride(v:MaybeVirtual): return v.__isExplicitOverride or __isOverride(v.inheritedVersion) if v.inheritedVersion != None else False
        
        this.inheritedVersion = module.find(f"{this.inheritedFromName}::{this.ownName}")
        this.isVirtual = __isVirtual(this)
        this.isOverride = __isOverride(this)


class Callable(ASTNode):
    class Parameter(ASTNode):
        def __init__(this, ownerName:str, ownName:str, location: SourceLocation, typeName:str):
            super().__init__(this, ownerName, ownName, location)
            this.typeName = typeName
            this.type:TypeInfo|None = None
            
        def latelink(this, module:Module):
            this.type = module.find(this.typeName)
            
        @staticmethod
        def allowMultiple():
            return True # In case they're nameless
            
    def __init__(this, ownerName:str, ownName:str, location: SourceLocation, returnTypeName:str, deleted:bool):
        super().__init__(this, ownerName, ownName, location)
        this.returnTypeName = returnTypeName
        this.deleted = deleted

    def latelink(this, module:Module):
        this.children = [i for i in this.children if isinstance(i, Callable.Parameter)]
        

# TODO implement:
#class GlobalFuncInfo
#class GlobalVarInfo - doubles as class static


class MemFuncInfo(MaybeVirtual, Callable):
    def __init__(this, ownerName:str, inheritedFromName:str|None, ownName:str, location: SourceLocation,
                visibility:Member.Visibility, isExplicitVirtual:bool, isExplicitOverride:bool,\
                returnTypeName:str, deleted:bool):
        super(MaybeVirtual).__init__(ownerName, ownName, location, visibility, isExplicitVirtual, isExplicitOverride)
        super(Callable).__init__(this, ownerName, ownName, location, returnTypeName, deleted)
        

class ConstructorInfo(Member, Callable):
    def __init__(this, owner:str, location: SourceLocation, deleted:bool, visibility:Member.Visibility):
        super(Member).__init__(owner, owner, location, visibility)
        super(Callable).__init__(owner, owner, location, None, deleted)


class DestructorInfo(MaybeVirtual, Callable):
    def __init__(this, owner:str, inheritedFromName:str|None, location: SourceLocation,
                visibility:Member.Visibility, isExplicitVirtual:bool, isExplicitOverride:bool,\
                deleted:bool):
        super(MaybeVirtual).__init__(owner, owner, location, visibility, isExplicitVirtual, isExplicitOverride)
        super(Callable).__init__(this, owner, owner, location, None, deleted)


class FieldInfo(Member):
    def __init__(this, ownerName:str, ownName:str, location:SourceLocation, visibility:Member.Visibility, typeName:str|None):
        super().__init__(this, ownerName, ownName, location, visibility)
        this.typeName = typeName
        this.type = None
        

class ParentInfo(Member):
    class Virtualness(str, Enum):
        NonVirtual       = "ParentInfo::Virtualness::NonVirtual"
        VirtualExplicit  = "ParentInfo::Virtualness::VirtualExplicit"
        VirtualInherited = "ParentInfo::Virtualness::VirtualInherited"

    def __init__(this, ownerTypeName:str, parentTypeName:str, location:SourceLocation, visibility:Member.Visibility, explicitlyVirtual:bool):
        super().__init__(this, ownerTypeName, None, location, visibility)
        this.parentTypeName = parentTypeName
        this.parentType = None
        this.explicitlyVirtual = explicitlyVirtual
    
    def link(this, module: Module):
        super().link(module)
        this.parentType = module.find(this.parentTypeName)

    def latelink(this, module: Module):
        if not this.explicitlyVirtual:
            def __isVirtual(ty:TypeInfo):
                return any((i.explicitlyVirtual for i in ty.immediateParents)) or \
                       any((__isVirtual(i.parentType) for i in ty.immediateParents))


class FriendInfo(Member):
    def __init__(this, ownerTypeName:str, friendedSymbolName:str, location:SourceLocation, visibility:Member.Visibility):
        super().__init__(this, ownerTypeName, None, location, visibility)
        this.friendedSymbolName = friendedSymbolName
        this.friendedSymbol = None
        
    def latelink(this, module: Module):
        this.friendedSymbol = module.find(this.friendedSymbolName)
