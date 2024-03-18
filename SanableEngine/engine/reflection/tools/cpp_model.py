import abc
from source_discovery import SourceFile
from enum import Enum
from typing import Callable





class _TemplatedName: # Local, relative only
    ConstValue = str
    def __init__(this, baseName: str, parameters: list["AbsName" | ConstValue]):
        this.baseName = baseName
        this.parameters = parameters
        
    def __str__(this):
        paramStrs = [str(i) for i in this.parameters]
        return f"{this.baseName}<{', '.join(paramStrs)}>"
AbsName = list[str | _TemplatedName]
        
Annotation = str

class SourceLocation:
    def __init__(this, file: SourceFile, line: int):
        this.file = file
        this.line = line

        



class Symbol:
    __metaclass__ = abc.ABCMeta
    
    def __init__(this, absName: AbsName):
        this.absName = absName
        this.definition: SourceLocation = None
        this.annotations: list[Annotation] = []
        
    # Executes low-to-high, returns callbacks and execution order
    def bind(this, module: "Module") -> list[tuple[Callable[[], None], int]]: return []
    
    def getReferencedTypes(this) -> list[AbsName]: return []


class Member(Symbol):
    __metaclass__ = abc.ABCMeta

    class Visibility:
        Public    = "MemberVisibility::Public"
        Protected = "MemberVisibility::Protected"
        Private   = "MemberVisibility::Private"
    
    def __init__(this, absName: AbsName, visibility: Visibility):
        super().__init__(absName)
        this.visibility = visibility
        this.owner: "TypeInfo" = None
        
    def bind(this, module):
        def __inner():
            this.owner = module.get(this.absName[:-1])
            this.owner.members.append(this)
        return super().bind(module) + [(__inner, 0)]
        
        
class Virtualizable(Member):
    __metaclass__ = abc.ABCMeta

    def __init__(this, absName: AbsName, visibility: Member.Visibility, explicitlyVirtual: bool, explicitlyOverride: bool):
        super().__init__(absName, visibility)
        this.explicitlyVirtual  = explicitlyVirtual
        this.explicitlyOverride = explicitlyOverride
        
        # Members deferred until bind stage
        this.overriddenMember: Member = None
        this.isVirtual: bool = None
    
    def bind(this, module):
        def __inner():
            # Attempt to locate equivalent member in parent
            this.overriddenMember = None
            for parentInfo in this.owner.parents:
                if parentInfo.parentType != None:
                    this.overriddenMember = parentInfo.parentType.getMember(this.relName, searchParents=True)
                    if this.overriddenMember != None:
                        break

            # Determine if virtual
            if this.overriddenMember != None:
                this.isVirtual = this.explicitlyVirtual or this.overriddenMember.isVirtual
        return super().bind(module) + [(__inner, 10)]
    

class Callable:
    def __init__(this, parameters: list["ParameterInfo"], isDeleted: bool = False):
        this.parameters = parameters
        this.isDeleted = isDeleted
        

class ParameterInfo:
    def __init__(this, typeName: AbsName, name: str):
        this.typeName = typeName
        this.name = name
        
        # Members deferred until bind stage
        this.type: "TypeInfo" = None

    def bind(this, module):
        def __inner(): this.type = module.get(this.typeName)
        return super().bind(module) + [(__inner, 10)]


class ConstructorInfo(Member, Callable):
    def __init__(this, absName: AbsName, visibility: Member.Visibility, parameters: list[ParameterInfo]):
        Member.__init__(this, absName, visibility)
        Callable.__init__(this, parameters)


class DestructorInfo(Virtualizable, Callable):
    def __init__(this, absName: AbsName, visibility: Member.Visibility):
        Virtualizable.__init__(this, absName, visibility)
        Callable.__init__(this, [])


class FieldInfo(Member):
    def __init__(this, absName: AbsName, visibility: Member.Visibility, declaredTypeName: AbsName):
        Member.__init__(this, absName, visibility)
        this.declaredTypeName = absName
        
        # Members deferred until bind stage
        this.declaredType: "TypeInfo" = None

    def bind(this, module):
        def __inner():
            this.declaredType = module.get(this.declaredTypeName)
        return super().bind(module) + [(__inner, 0)]
    

class ParentInfo:
    class Virtualness(str, Enum):
        NonVirtual       = "ParentInfo::Virtualness::NonVirtual"
        VirtualExplicit  = "ParentInfo::Virtualness::VirtualExplicit"
        VirtualInherited = "ParentInfo::Virtualness::VirtualInherited"

    def __init__(this, childTypeName: AbsName, parentTypeName: AbsName, visibility: Member.Visibility, isVirtual: bool):
        this.childTypeName = childTypeName
        this.parentTypeName = parentTypeName
        this.isVirtual = isVirtual
        
        # Members deferred until bind stage
        this.childType: "TypeInfo" = None
        this.parentType: "TypeInfo" = None
        if this.isVirtual: this.virtualness = ParentInfo.Virtualness.VirtualExplicit
        else:              this.virtualness = ParentInfo.Virtualness.NonVirtual
            
    def bind(this, module):
        def __inner_getParentType():
            this.childType = module.get(this.childTypeName)
            this.childType.parents.append(this)
            this.parentType = module.get(this.parentTypeName)
            this.parentType.children.append(this)
            
        def __inner_propagateVirtualness():
            def __prop(tgt: "TypeInfo"):
                tgt.implicitVirtualParents.append(this)
                for i in tgt.children: __prop(i.childType)
            for i in this.childType.children: __prop(i.childType)
            
        return super().bind(module) + [
            (__inner_getParentType, 0),
            (__inner_propagateVirtualness, 10)
        ]


class FriendInfo:
    def __init__(this, exposingTypeName: AbsName, friendedSymbolName: AbsName):
        this.exposingTypeName = exposingTypeName
        this.friendedSymbolName = friendedSymbolName
        
        # Members deferred until bind stage
        this.exposingType: "TypeInfo" = None
        this.friendedSymbol: "TypeInfo" = None
            
    def bind(this, module):
        def __inner():
            this.exposingType = module.get(this.exposingTypeName)
            this.exposingType.friends.append(this)
            this.friendedSymbol = module.get(this.friendedSymbolName)
        return super().bind(module) + [(__inner, 0)]
    

class TypeInfo(Symbol):
    def __init__(this, absName: AbsName):
        super().__init__(absName)
        this.children: list[ParentInfo] = []
        this.parents: list[ParentInfo] = []
        this.members: list[Member] = []
        
    def getMember(this, absName: AbsName) -> Member:
        for i in this.members:
            if i.absName == absName: return i
        return None
        
        
class Module:
    def __init__(this):
        this.__symbols: dict[AbsName, Symbol] = []

    def get(this, absName: AbsName) -> Symbol:
        if absName in this.__symbols.keys(): return this.__symbols[absName]
        else: return None
    
    def put(this, symbol: Symbol):
        existing = this.get(symbol.absName)
        if existing != None:
            # A symbol already exists with this name: check we aren't overwriting
            if symbol.definition != None:
                assert existing.definition == None, f"Tried to register {symbol.absName}, but a definition was already registered!"
                this.__symbols[symbol.absName] = symbol
        else:
            # No symbol exists with this name: just put into symbols
            this.__symbols[symbol.absName] = symbol

    def bind(this):
        binders = []
        for i in this.__symbols.values(): binders += i.bind()
        binders.sort(key=lambda v: v[1])
        for i in binders: i()
