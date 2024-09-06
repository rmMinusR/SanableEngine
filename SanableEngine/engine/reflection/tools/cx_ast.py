import abc
from enum import Enum
from functools import cached_property
from symtable import Symbol
from source_discovery import SourceFile
import config
import typing

class SourceLocation:
    def __init__(this, file: SourceFile, line:int, column:int):
        this.file = file
        this.line = line
        this.column = column
        
    def __repr__(this):
        return f"{this.file.name} : L{this.line}.C{this.column}"


class SymbolPath:
    BasicSegment = str
    class TemplatedSegment:
        def __init__(this, base:str):
            this.base = base
            this.templateParams:list[ str|SymbolPath ] = [] # SymbolPath for type arguments, str for values (int, char, enum, etc)
            
        def __str__(this): return this.base+"<" + ", ".join(str(i) for i in this.templateParams) + ">"
        def __repr__(this): return this.__str__()

        def __eq__(this, rhs):
            if not isinstance(rhs, SymbolPath.TemplatedSegment): return False
            if this.base != rhs.base or len(this.templateParams) != len(rhs.templateParams): return False
            return all(this.templateParams[i]==rhs.templateParams[i] for i in range(len(this.templateParams)))
    class Anonymous:
        def __init__(this, location:SourceLocation, _type:str = "anonymous"):
            this.location = location
            this._type = _type
            
        def __str__(this): return f"({this._type} at {this.location})"
        def __repr__(this): return this.__str__()
        
        def __eq__(this, rhs):
            return isinstance(rhs, SymbolPath.Anonymous) and this.location == rhs.location
    class CallParameterized: # Used to disambiguate method groups with argument or constness overloading
        def __init__(this, base:"SymbolPath.SegmentAny", args:"list[SymbolPath]", affixes:list[str]):
            this.base = base
            this.args = args
            this.affixes = affixes # const/volatile. NOT virtual/override.
            this.affixes.sort()
            
        def __str__(this):
            out = this.base+"(" + ", ".join([str(i) for i in this.args]) + ")"
            if len(this.affixes) > 0: out += " "+" ".join(this.affixes)
            return out
        def __repr__(this): return this.__str__()

        def __eq__(this, rhs):
            if not isinstance(rhs, SymbolPath.CallParameterized): return False
            if this.base != rhs.base or len(this.args) != len(rhs.args) or len(this.affixes) != len(rhs.affixes): return False
            return all(this.affixes[i]==rhs.affixes[i] for i in range(len(this.affixes))) \
               and all(this.args[i]==rhs.args[i] for i in range(len(this.args)))

    SegmentAny = BasicSegment|TemplatedSegment|Anonymous|CallParameterized

    def __init__(this):
        this.__parts:list[SymbolPath.SegmentAny] = []

    def __str__(this): return "::" + "::".join([str(i) for i in this.__parts])
    def __repr__(this): return this.__str__()
    
    def __hash__(this): return hash(this.__str__())

    def __div__(this, rhs:"SymbolPath.SegmentAny"):
        out = SymbolPath()
        out.__parts.extend(this.__parts)
        out.__parts.append(rhs)
        return out
    def __add__(this, rhs): return this.__div__(rhs)
    
    def __eq__(this, rhs):
        if not isinstance(rhs, SymbolPath): return False
        if len(this.__parts) != len(rhs.__parts): return False
        return all(this.__parts[i]==rhs.__parts[i] for i in range(len(this.__parts)))
    
    @cached_property
    def parent(this) -> "SymbolPath|None":
        if len(this.__parts) > 1:
            out = SymbolPath()
            out.__parts = this.__parts[:-1]
            return out
        else: return None
        
    @cached_property
    def ownName(this): return this.__parts[-1]

    @cached_property
    def name(this): return str(this.__parts[-1])
    
    @cached_property # NOTE: Doesn't do visibility checks
    def referenceable(this): return any((isinstance(i, SymbolPath.Anonymous) for i in this.__parts))

    #@staticmethod
    #def naive_parse(raw:str): # TODO gets completely screwed if someone does a boolean less-than compare. Scrap?
    #    out = SymbolPath()
    #    braces = ("()", "<>", "{}", "[]", '""', "''") # Hope nobody uses trigraphs...
    #    
    #    if raw.startswith("::"): raw = raw[2:]
    #
    #    while len(raw) > 0:
    #        braceStack = []
    #        for idx in range(1, len(raw)):
    #            if len(braceStack) > 0 and any( braceStack[-1]==brace[0] and raw[idx] == brace[1] for brace in braces ):
    #                # Closing brace or quote
    #                braceStack.pop()
    #            elif any( raw[idx] == brace[0] for brace in braces ):
    #                # Opening brace or quote
    #                braceStack.append(raw[idx])
    #            elif len(braceStack)==0 and idx<len(raw)-1 and raw[idx:idx+2] == "::":
    #                break
    #        out.__parts.append(raw[:idx])
    #        raw = raw[idx+2:]
    #            
    #    return out



class ASTNode:
    __metaclass__ = abc.ABCMeta

    def __init__(this, path:SymbolPath|None, location: SourceLocation, isDefinition:bool):
        this.path = path
        this.transient:bool = False # Used by implicits added during the link phase

        this.declarationLocations:list[SourceLocation] = []
        this.definitionLocation:SourceLocation = None
        if isDefinition: this.definitionLocation = location
        else: this.declarationLocations.append(location)
        
        this.owner:ASTNode|None = None
        this.children = []

    def __str__(this):
        typeStr = str(type(this))
        typeStr = typeStr[len("<class '"):-len("'>")].split(".")[-1]
        return f"{this.path} ({typeStr})"
    
    def __repr__(this):
        return str(this)
    
    def merge(this, new:"ASTNode"): # Called on existing instance
        # Already aware of this symbol: check that previous record was a declaration
        if new.definitionLocation != None:
            assert this.definitionLocation == None, f"Node {this} defined multiple times!"

        # Merge locational data
        this.definitionLocation = new.definitionLocation
        this.declarationLocations.extend(new.declarationLocations)
        
        # Move over children
        this.children.extend(new.children)
        for i in new.children: i.owner = this

    @staticmethod
    def allowMultiple():
        return False

    def link(this, module: "Module"):
        pass
            
    def latelink(this, module: "Module"):
        pass


class Module:
    def __init__(this):
        this.contents:dict[SymbolPath, ASTNode|list[ASTNode]] = dict()
        this.byType:dict[type, list[ASTNode]] = dict()
        this.__linked = False
        this.__linking = False
        this.__concurrentlyAdded = []
        
    def __getstate__(this):
        out = [i for i in this.contents.values() if isinstance(i, ASTNode) and not i.transient] \
            + [[i for i in grp if not i.transient] for grp in this.contents.values() if isinstance(grp, list)]
        out.sort(key=lambda node: str(node.path if isinstance(node, ASTNode) else node[0].path))
        return out
    
    def __setstate__(this, vals):
        this.contents = dict()
        this.byType = dict()
        this.__concurrentlyAdded = []

        def _put_byType(i:ASTNode):
            if type(i) not in this.byType.keys(): this.byType[type(i)] = []
            this.byType[type(i)].append(i)

        for i in vals:
            if isinstance(i, ASTNode):
                this.contents[i.path] = i
                _put_byType(i)
            else:
                if not i[0].path in this.contents.keys(): this.contents[i[0].path] = []
                this.contents[i[0].path].extend(i)
                for j in i: _put_byType(j)

    def register(this, node:ASTNode):
        if this.__linking:
            this.__concurrentlyAdded.append(node) # Defer
        else:
            assert not this.__linked, "Cannot register new nodes after linking"
            this.__registerInternal(node)
    
    def __registerInternal(this, node:ASTNode):
        # Place in fast-lookup contents tree
        if not node.allowMultiple():
            # Aware of symbol: merge into existing entry
            if node.path in this.contents.keys(): this.contents[node.path].merge(node)
            
            # Not aware of symbol: nothing to do
            else: this.contents[node.path] = node
        else:
            # Duplicate-allowed symbols are complete and utter chaos
            if not node.path in this.contents.keys(): this.contents[node.path] = []
            this.contents[node.path].append(node)
        
        # Place in by-type lookup
        if not type(node) in this.byType.keys(): this.byType[type(node)] = []
        this.byType[type(node)].append(node)

    def remove(this, node:ASTNode):
        del this.contents[node.path]
        this.byType[type(node)].remove(node)
        if node.owner != None: node.owner.children.remove(node)

    def linkAll(this):
        this.__linked = True
        this.__linking = True
        
        def _reducing_invoke(obj, op):
            if isinstance(obj, list):
                for i in obj: op(i)
            else: op(obj)
        
        # Nasty hack to prevent symbol duplication
        for v in this.contents.values(): _reducing_invoke(v, lambda o: setattr(o, "children", []))
        def _associate_owner_with_child(o):
            if o.owner != None: o.owner.children.append(o)
        for v in this.contents.values(): _reducing_invoke(v, _associate_owner_with_child)
        
        # Link explicit symbols
        for v in this.contents.values(): _reducing_invoke(v, lambda o:o.link(this))
        
        # Link implicit symbols added in link()
        while len(this.__concurrentlyAdded) > 0:
            node = this.__concurrentlyAdded.pop(0)
            this.__registerInternal(node)
            
            # Handle parent/children connection
            parent = this.find(node.path.parent)
            if parent != None:
                parent.children.append(node)
                node.owner = parent
                
            # Link
            _reducing_invoke(node, lambda o:o.link(this))
            
        # Late-link explicit symbols
        for v in this.contents.values(): _reducing_invoke(v, lambda o:o.latelink(this))
        
        # Fully link implicit symbols added in latelink()
        while len(this.__concurrentlyAdded) > 0:
            node = this.__concurrentlyAdded.pop(0)
            this.__registerInternal(node)
            
            # Handle parent/children connection
            parent = this.find(node.path.parent)
            if parent != None:
                parent.children.append(node)
                node.owner = parent
                
            # Link and late-link
            _reducing_invoke(node, lambda o:o.link(this))
            _reducing_invoke(node, lambda o:o.latelink(this))
        
        this.__linking = False
        
    def find(this, path:SymbolPath) -> ASTNode:
        assert this.__linked, "Can only be called after or during linking"
        try: return this.contents[path]
        except KeyError: return None


class Namespace(ASTNode):
    def __init__(this, path:SymbolPath, location: SourceLocation):
        ASTNode.__init__(this, path, location, False)
        
    #@staticmethod
    #def allowMultiple():
    #    return True


class Annotation(ASTNode):
    def __init__(this, ownerPath:SymbolPath, text:str, location:SourceLocation):
        ASTNode.__init__(this, ownerPath+SymbolPath.Anonymous(location, _type=f"annotation '{text}'"), location, True)
        this.text = text
        
    @staticmethod
    def allowMultiple():
        return True
    

class TypeInfo(ASTNode):
    def __init__(this, path:SymbolPath, location: SourceLocation, isDefinition:bool, isAbstract:bool):
        ASTNode.__init__(this, path, location, isDefinition)
        this.isAbstract = isAbstract
        
    @property
    def immediateParents(this):
        return (i for i in this.children if isinstance(i, ParentInfo))

    def link(this, module:"Module"):
        if this.definitionLocation != None: # Somehow this is being called multiple times per type??? This causes multiple ctors to be registered in a single run but somehow they sneak in after verification...
            # Implicit default ctor
            if not any((isinstance(i, ConstructorInfo) for i in this.children)):
                ctorPathPart = SymbolPath.CallParameterized(str(this.path.ownName), [], [])
                implicitDefaultCtor = ConstructorInfo(this.path+ctorPathPart, this.definitionLocation, True, False, True, Member.Visibility.Public)
                implicitDefaultCtor.transient = True
                module.register(implicitDefaultCtor)
                this.children.append(implicitDefaultCtor)

            # Implicit default ctor
            if not any((isinstance(i, DestructorInfo) for i in this.children)):
                dtorPathPart = SymbolPath.CallParameterized(f"~{this.path.ownName}", [], [])
                implicitDefaultDtor = DestructorInfo(this.path+dtorPathPart, this.definitionLocation, True, Member.Visibility.Public, False, False, False, True)
                implicitDefaultDtor.transient = True
                module.register(implicitDefaultDtor)
                this.children.append(implicitDefaultDtor)
        
        super().link(module)

    def find(this, memberName, searchParents=False) -> "Member":
        for i in this.children:
            if isinstance(i, Member) and i.path.ownName == memberName:
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

    def isFriended(this, selector:typing.Callable[["FriendInfo"], bool]):
        return any((
            isinstance(i, FriendInfo) and selector(i)
            for i in this.children
        ))
        
     
        
class Member(ASTNode):
    
    class Visibility(str, Enum):
        Public    = "MemberVisibility::Public"
        Protected = "MemberVisibility::Protected"
        Private   = "MemberVisibility::Private"

    def __init__(this, path:SymbolPath, location:SourceLocation, isDefinition:bool, visibility:Visibility):
        ASTNode.__init__(this, path, location, isDefinition)
        this.visibility = visibility
        this.owner = None
    

class MaybeVirtual(Member):
    __metaclass__ = abc.ABCMeta
    
    def __init__(this, path:SymbolPath, location:SourceLocation, isDefinition:bool, visibility:Member.Visibility, isExplicitVirtual:bool, isExplicitOverride:bool):
        Member.__init__(this, path, location, isDefinition, visibility)
        this.__isExplicitVirtual = isExplicitVirtual
        this.__isExplicitOverride = isExplicitOverride
        this.inheritedFrom = None
        this.inheritedVersion = None
        this.isVirtual = None
        this.isOverride = None
        
    def latelink(this, module: Module):
        this.inheritedVersion = this.owner.findInParents(this.path.ownName)
        
        def __isVirtual (v:MaybeVirtual): return v.__isExplicitVirtual  or (__isVirtual (v.inheritedVersion) if v.inheritedVersion != None else False)
        def __isOverride(v:MaybeVirtual): return v.__isExplicitOverride or (__isOverride(v.inheritedVersion) if v.inheritedVersion != None else False)
        this.isVirtual = __isVirtual(this)
        this.isOverride = __isOverride(this)


class Callable(ASTNode):
    class Parameter(ASTNode):
        def __init__(this, path:SymbolPath, location:SourceLocation, typeName:str):
            ASTNode.__init__(this, path.parent+SymbolPath.Anonymous(location, _type=f"parameter {typeName} {path.ownName}"), location, True)
            this.typeName = typeName
            this.type:TypeInfo|None = None

        def latelink(this, module:Module):
            this.type = module.find(this.typeName)

        @staticmethod
        def allowMultiple():
            return True
            
    def __init__(this, path:SymbolPath, location:SourceLocation, isDefinition:bool, returnTypeName:str, deleted:bool, inline:bool):
        ASTNode.__init__(this, path, location, isDefinition)
        this.returnTypeName = returnTypeName
        this.deleted = deleted
        this.inline = inline
        if inline and isDefinition: this.declarationLocations.append(location)

    def link(this, module:Module):
         super().link(module)
         assert isinstance(this.path.ownName, SymbolPath.CallParameterized)

    @property
    def parameters(this) -> list[Parameter]:
        return [i for i in this.children if isinstance(i, Callable.Parameter)]
   

class StaticFuncInfo(Callable, Member):
    def __init__(this, path:SymbolPath, location:SourceLocation, isDefinition:bool, visibility:Member.Visibility, returnTypeName:str, deleted:bool, inline:bool):
        Callable.__init__(this, path, location, isDefinition, returnTypeName, deleted, inline)
        Member.__init__(this, path, location, isDefinition, visibility)
        

class GlobalFuncInfo(Callable):
    def __init__(this, path:SymbolPath, location:SourceLocation, isDefinition:bool, returnTypeName:str, deleted:bool, inline:bool):
        Callable.__init__(this, path, location, isDefinition, returnTypeName, deleted, inline)
    

class MemFuncInfo(MaybeVirtual, Callable):
    def __init__(this, path:SymbolPath, location:SourceLocation, isDefinition:bool,
                visibility:Member.Visibility, isExplicitVirtual:bool, isExplicitOverride:bool,\
                returnTypeName:str, deleted:bool, inline:bool,
                isThisObjConst:bool, isThisObjVolatile:bool):
        MaybeVirtual.__init__(this, path, location, isDefinition, visibility, isExplicitVirtual, isExplicitOverride)
        Callable    .__init__(this, path, location, isDefinition, returnTypeName, deleted, inline)
        this.isThisObjConst = isThisObjConst
        this.isThisObjVolatile = isThisObjVolatile
        

class ConstructorInfo(Member, Callable):
    def __init__(this, path:SymbolPath, location:SourceLocation, isDefinition:bool, deleted:bool, inline:bool, visibility:Member.Visibility):
        Member  .__init__(this, path, location, isDefinition, visibility)
        Callable.__init__(this, path, location, isDefinition, None, deleted, inline)


class DestructorInfo(MaybeVirtual, Callable):
    def __init__(this, path:SymbolPath, location:SourceLocation, isDefinition:bool,
                visibility:Member.Visibility, isExplicitVirtual:bool, isExplicitOverride:bool,\
                deleted:bool, inline:bool):
        MaybeVirtual.__init__(this, path, location, isDefinition, visibility, isExplicitVirtual, isExplicitOverride)
        Callable    .__init__(this, path, location, isDefinition, None, deleted, inline)


class GlobalVarInfo(ASTNode):
    def __init__(this, path:SymbolPath, location: SourceLocation, isDefinition:bool, typeName:str|None):
        ASTNode.__init__(this, path, location, isDefinition)
        this.typeName = typeName
        this.type = None
        
    def link(this, module:Module):
        super().link(module)
        this.type = module.find(this.typeName)


class StaticVarInfo(Member):
    def __init__(this, path:SymbolPath, location:SourceLocation, isDefinition:bool, visibility:Member.Visibility, typeName:str|None):
        Member.__init__(this, path, location, isDefinition, visibility)
        this.typeName = typeName
        this.type = None
        
    def link(this, module:Module):
        super().link(module)
        this.type = module.find(this.typeName)


class FieldInfo(Member):
    def __init__(this, path:SymbolPath, location:SourceLocation, visibility:Member.Visibility, typeName:str|None):
        Member.__init__(this, path, location, True, visibility)
        this.typeName = typeName
        this.type = None
        
    def link(this, module:Module):
        super().link(module)
        this.type = module.find(this.typeName)
        

class ParentInfo(Member):
    class Virtualness(str, Enum):
        NonVirtual       = "ParentInfo::Virtualness::NonVirtual"
        VirtualExplicit  = "ParentInfo::Virtualness::VirtualExplicit"
        VirtualInherited = "ParentInfo::Virtualness::VirtualInherited"

    def __init__(this, ownerPath:SymbolPath, parentTypePath:SymbolPath, location:SourceLocation, visibility:Member.Visibility, explicitlyVirtual:bool):
        Member.__init__(this, ownerPath+SymbolPath.Anonymous(location, _type=f"parent {parentTypePath}"), location, True, visibility)
        this.parentTypePath = parentTypePath
        this.parentType = None
        this.explicitlyVirtual = explicitlyVirtual
        #this.isVirtuallyInherited = None
    
    def link(this, module: Module):
        super().link(module)
        this.parentType = module.find(this.parentTypePath)

    def latelink(this, module: Module):
        #if this.explicitlyVirtual:
        #    this.isVirtuallyInherited = True
        #else:
        #    def __immediateParents(ty:TypeInfo): return (i for i in ty if isinstance(i, ParentInfo))
        #    def __isVirtuallyInherited(p:ParentInfo):
        #        
        #    def __isVirtual(ty:TypeInfo):
        #        return any(( i.explicitlyVirtual for i in __immediateParents(ty) )) or \
        #               any(( __isVirtual(i.parentType) for i in __immediateParents(ty) ))
        #    this.isVirtuallyInherited = __isVirtual(this)
        pass


class FriendInfo(Member):
    def __init__(this, ownerPath:SymbolPath, friendedSymbolPath:SymbolPath, location:SourceLocation, visibility:Member.Visibility):
        Member.__init__(this, ownerPath+SymbolPath.Anonymous(location, _type=f"friend {this.friendedSymbolName}"), location, True, visibility)
        this.friendedSymbolPath = friendedSymbolPath
        this.friendedSymbol = None

    def latelink(this, module: Module):
        this.friendedSymbol = module.find(this.friendedSymbolPath)


class TemplateParameter(ASTNode):
    def __init__(this, path:SymbolPath, location:SourceLocation, paramType:str, defaultValue:str|None): # paramType is one of: typename, concept, class, struct, int... (or any other templatable)
        this.paramName = path.ownName
        ASTNode.__init__(this, path.parent+SymbolPath.Anonymous(location, _type=f"template parameter {paramType} {path.ownName}"), location, True)
        # FIXME parameter index!
        this.paramType = paramType
        this.defaultValue = defaultValue
