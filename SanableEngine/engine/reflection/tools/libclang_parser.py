import cpp_model
from clang.cindex import AccessSpecifier
from clang.cindex import *


def parse(module: cpp_model.Module, cursor: Cursor):
    # Namespaces: recurse
    if cursor.kind == CursorKind.NAMESPACE:
        for i in cursor.get_children(): parse(module, i)
            
    # Otherwise, defer then recurse
    elif cursor.kind in factories.keys():
        obj: cpp_model.Symbol = factories[cursor.kind](cursor)
        if obj == None: return
        module.put(obj)
        for i in cursor.get_children(): parse(module, i)

    # Safety warning
    elif cursor.kind not in ignoredSymbols:
        print(f"WARNING: Unhandled")


factories = {
    
}


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