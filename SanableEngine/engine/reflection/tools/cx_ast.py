import abc
from enum import Enum
from functools import cached_property
from source_discovery import SourceFile
import config

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
        typeStr = str(type(this))
        typeStr = typeStr[len("<class '"):-len("'>")].split(".")[-1]
        return f"{this.path} ({typeStr})"
    
    @cached_property
    def path(this):
        if this.ownerName != None: return f"{this.ownerName}::{this.ownName}"
        else: return f"::{this.ownName}"
    
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
        out.sort(key=lambda node: node.path)
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
            #assert node.path not in this.contents.keys(), f"Tried to register {node} twice!"
            if node.path in this.contents.keys(): config.logger.warn(f"Node {node} registered multiple times!")
            this.contents[node.path] = node
        else:
            if not node.path in this.contents.keys(): this.contents[node.path] = []
            this.contents[node.path].append(node)
        
        # Place in by-type lookup
        if not type(node) in this.byType.keys(): this.byType[type(node)] = []
        this.byType[type(node)].append(node)
            
    def linkAll(this):
        this.__linked = True
        for v in this.contents.values(): v.link(this)
        for v in this.contents.values(): v.latelink(this)
        
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
    def __init__(this, ownerName:str|None, ownName:str, location: SourceLocation, isAbstract:bool):
        super().__init__(ownerName, ownName, location)
        this.isAbstract = isAbstract
        
    @property
    def immediateParents(this):
        return (i for i in this.children if isinstance(i, ParentInfo))
    
    def find(this, memberName, searchParents=False) -> "Member":
        for i in this.children:
            if isinstance(i, Member) and i.ownName == memberName:
                return i
            
        if searchParents:
            return this.findInParents(memberName)
            
        return None
    
    def findInParents(this, memberName) -> "Member":
        for i in this.children:
            if isinstance(i, ParentInfo):
                potentialMatch = i.parentType.find(memberName, searchParents=True)
                if potentialMatch != None: return potentialMatch
        return None
        
     
        
class Member(ASTNode):
    
    class Visibility(str, Enum):
        Public    = "MemberVisibility::Public"
        Protected = "MemberVisibility::Protected"
        Private   = "MemberVisibility::Private"

    def __init__(this, ownerName:str, ownName:str, location:SourceLocation, visibility:Visibility):
        ASTNode.__init__(this, ownerName, ownName, location)
        this.visibility = visibility
        this.owner = None
        
    def link(this, module:"Module"):
        super().link(module)
        this.owner = module.find(this.ownerName)
    

class MaybeVirtual(Member):
    __metaclass__ = abc.ABCMeta
    
    def __init__(this, ownerName:str, ownName:str, location: SourceLocation, visibility:Member.Visibility, isExplicitVirtual:bool, isExplicitOverride:bool):
        Member.__init__(this, ownerName, ownName, location, visibility)
        this.__isExplicitVirtual = isExplicitVirtual
        this.__isExplicitOverride = isExplicitOverride
        this.inheritedFrom = None
        this.inheritedVersion = None
        this.isVirtual = None
        this.isOverride = None
        
    def latelink(this, module: Module):
        this.inheritedVersion = this.owner.findInParents(this.ownName)
        
        def __isVirtual (v:MaybeVirtual): return v.__isExplicitVirtual  or __isVirtual (v.inheritedVersion) if v.inheritedVersion != None else False
        def __isOverride(v:MaybeVirtual): return v.__isExplicitOverride or __isOverride(v.inheritedVersion) if v.inheritedVersion != None else False
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
        ASTNode.__init__(this, ownerName, ownName, location)
        this.returnTypeName = returnTypeName
        this.deleted = deleted

    def latelink(this, module:Module):
        this.children = [i for i in this.children if isinstance(i, Callable.Parameter)]
        

# TODO implement:
#class GlobalFuncInfo
#class GlobalVarInfo - doubles as class static


class MemFuncInfo(MaybeVirtual, Callable):
    def __init__(this, ownerName:str, ownName:str, location: SourceLocation,
                visibility:Member.Visibility, isExplicitVirtual:bool, isExplicitOverride:bool,\
                returnTypeName:str, deleted:bool):
        MaybeVirtual.__init__(this, ownerName, ownName, location, visibility, isExplicitVirtual, isExplicitOverride)
        Callable.__init__(this, ownerName, ownName, location, returnTypeName, deleted)
        

class ConstructorInfo(Member, Callable):
    def __init__(this, owner:str, location: SourceLocation, deleted:bool, visibility:Member.Visibility):
        Member.__init__(this, owner, owner, location, visibility)
        Callable.__init__(this, owner, owner, location, None, deleted)


class DestructorInfo(MaybeVirtual, Callable):
    def __init__(this, owner:str, location: SourceLocation,
                visibility:Member.Visibility, isExplicitVirtual:bool, isExplicitOverride:bool,\
                deleted:bool):
        MaybeVirtual.__init__(this, owner, owner, location, visibility, isExplicitVirtual, isExplicitOverride)
        Callable.__init__(this, owner, owner, location, None, deleted)


class FieldInfo(Member):
    def __init__(this, ownerName:str, ownName:str, location:SourceLocation, visibility:Member.Visibility, typeName:str|None):
        Member.__init__(this, ownerName, ownName, location, visibility)
        this.typeName = typeName
        this.type = None
        

class ParentInfo(Member):
    class Virtualness(str, Enum):
        NonVirtual       = "ParentInfo::Virtualness::NonVirtual"
        VirtualExplicit  = "ParentInfo::Virtualness::VirtualExplicit"
        VirtualInherited = "ParentInfo::Virtualness::VirtualInherited"

    def __init__(this, ownerTypeName:str, parentTypeName:str, location:SourceLocation, visibility:Member.Visibility, explicitlyVirtual:bool):
        Member.__init__(this, ownerTypeName, None, location, visibility)
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
        Member.__init__(this, ownerTypeName, None, location, visibility)
        this.friendedSymbolName = friendedSymbolName
        this.friendedSymbol = None
        
    def latelink(this, module: Module):
        this.friendedSymbol = module.find(this.friendedSymbolName)
