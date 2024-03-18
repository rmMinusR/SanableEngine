from typing import Callable
import cpp_model
import config
from clang.cindex import AccessSpecifier, TemplateArgumentKind
from clang.cindex import *


def buildAbsName(target: Cursor) -> cpp_model.AbsName:
    out: list = [] # NOTE: This is in reverse order, corrected at end of function
    
    while type(target) == type(None) or target.kind == CursorKind.TRANSLATION_UNIT:
        part = target.displayname
        
        # Strip leading C-style record type specifier
        def stripexact(val: str, leading: str): return val[len(leading):] if val.startswith(leading) else val
        part = stripexact(part, "enum ")
        part = stripexact(part, "class ") # Since this is after "enum" it will also catch "enum class"
        part = stripexact(part, "struct ")

        # Handle template args
        if target.get_num_template_arguments() > 0:
            templateArgs = []
            for idx in range(target.get_num_template_arguments()):
                kind: TemplateArgumentKind = target.get_template_argument_kind(idx)
                if kind == TemplateArgumentKind.TYPE:
                    templateArgs.append(buildAbsName(target.get_template_argument_type(idx)))
                elif kind == TemplateArgumentKind.INTEGRAL:
                    templateArgs.append(target.get_template_argument_value(idx))
                else:
                    assert False, f"Template argument of kind {kind} not yet implemented" # TODO
            part = cpp_model._TemplatedName(part, templateArgs)

        out.append(part)
        target = target.semantic_parent
    
    out.reverse()
    return cpp_model.AbsName(out)


def parse(module: cpp_model.Module, cursor: Cursor):
    # Namespaces: recurse
    if cursor.kind == CursorKind.NAMESPACE:
        for i in cursor.get_children(): parse(module, i)
            
    # Otherwise, defer then recurse
    elif cursor.kind in factories.keys():
        obj = None
        for f in factories:
            res = f(cursor)
            if res != None:
                assert obj == None or i == None, f"Multiple factories returned results for the same symbol ({buildAbsName(cursor)})"
            obj = res

        if obj == None: return
        module.put(obj)
        for i in cursor.get_children(): parse(module, i)

    # Safety warning
    elif cursor.kind not in ignoredSymbols:
        config.logger.warn(f"Unhandled symbol {cursor.kind} {buildAbsName(cursor)}")


factories: list[ Callable[[Cursor], cpp_model.Symbol|None] ] = [
    
    buildParameterInfo,
    
    buildConstructorInfo,
    buildDestructorInfo,
    buildFieldInfo,
    
    buildParentInfo,
    buildFriendInfo,
    
]


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