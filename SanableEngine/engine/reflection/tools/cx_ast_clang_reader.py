import cx_ast
import cx_ast_tooling
from source_discovery import *
import config
import timings
import clang.cindex
import argparse

from collections.abc import Iterator
import typing


class ClangParseContext(cx_ast_tooling.ASTParser):
    @staticmethod
    def argparser_add_defaults(parser: argparse.ArgumentParser):
        cx_ast_tooling.ASTParser.argparser_add_defaults(parser)

    def __init__(this, project:Project, args:argparse.Namespace, parser_version_hash):
        super().__init__(project, args, parser_version_hash)

    def configure(this):
        return super().configure()

    def ingest(this):
        timings.switchTask(timings.TASK_ID_WALK_AST_INTERNAL)

        # Do removal of outdated/removed symbols
        for source in this.diff.outdated+this.diff.removed:
            symbolsToRemove = [i for i in this.module.contents.values() if i.sourceFile == source]
            for i in symbolsToRemove:
                del this.module.symbols[i.path]

        # Do parsing of outdated/new symbols
        for source in this.diff.outdated+this.diff.new:
            config.logger.info(f"Parsing {source}")
            for cursor in ClangParseContext._getChildren(source.parse()):
                # Only capture what's in the current file
                cursorFilePath = cursor.location.file.name.replace(os.altsep, os.sep)
                if source.path == cursorFilePath: this.__ingestCursor(None, cursor)
                
        this.module.linkAll()


    @staticmethod
    def _getChildren(cursor: Cursor) -> Iterator[Cursor]:
        """
        Wrapper that helps us time most Clang operations
        """
        return timings.timeScoped(lambda: cursor.get_children(), timings.TASK_ID_WALK_AST_EXTERNAL)


    def __isOurs(this, cursor: Cursor):
        cursorFilePath = cursor.location.file.name.replace(os.altsep, os.sep)
        return any((cursorFilePath == i.path for i in this.project.files))


    factory_t = typing.Callable[[cx_ast.ASTNode|None, Cursor, Project], cx_ast.ASTNode|None]
    factories:dict[CursorKind, factory_t] = dict()

    def __ingestCursor(this, parent:cx_ast.ASTNode|None, cursor:Cursor):
        # No need to check if it's ours: we're guaranteed it is, if a parent is
        kind = cursor.kind
        if kind == CursorKind.NAMESPACE:
            for i in ClangParseContext._getChildren(cursor): this.__ingestCursor(parent, i) # TODO fully-qualified name handling?
        elif kind in ClangParseContext.factories.keys():
            result = ClangParseContext.factories[kind](parent, cursor, this.project)
            if result != None:
                for child in ClangParseContext._getChildren(cursor): this.__ingestCursor(result, child)
                this.module.register(result)
        else:
            config.logger.debug(f"Skipping symbol of unhandled kind {kind}")
            
def ASTFactory(*cursorTypes:CursorKind):
    """
    Helper to auto-register factory functions
    """
    def registrar(func:ClangParseContext.factory_t):
        for i in cursorTypes:
            assert i not in ClangParseContext.factories.keys()
            ClangParseContext.factories[i] = func
        return func
    return registrar



def makeSourceLocation(cursor:Cursor, project:Project):
    clang_loc:SourceLocation = cursor.location # Clang
    sourceFile = project.getFile(clang_loc.file.name)
    assert sourceFile != None
    return cx_ast.SourceLocation(sourceFile, clang_loc.line, clang_loc.column)

def makeVisibility(cursor:Cursor):
    return {
            clang.cindex.AccessSpecifier.PUBLIC   : cx_ast.Member.Visibility.Public,
            clang.cindex.AccessSpecifier.PROTECTED: cx_ast.Member.Visibility.Protected,
            clang.cindex.AccessSpecifier.PRIVATE  : cx_ast.Member.Visibility.Private,
            clang.cindex.AccessSpecifier.INVALID  : cx_ast.Member.Visibility.Public # TODO imply that it's public but only because it isn't nested?
        }[cursor.access_specifier]

def isExplicitVirtual(cursor:Cursor):
    return cursor.is_virtual_method() or cursor.is_pure_virtual_method()

def isExplicitOverride(cursor:Cursor):
    return any([i.kind == CursorKind.CXX_OVERRIDE_ATTR for i in ClangParseContext._getChildren(cursor)])



@ASTFactory(
    CursorKind.CLASS_DECL, CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION, # TODO only if fully specialized, else defer to CLASS_TEMPLATE handler
	CursorKind.STRUCT_DECL, CursorKind.UNION_DECL
    # TODO Removed temporarily: CursorKind.CLASS_TEMPLATE
)
def factory_TypeInfo(lexicalParent:cx_ast.ASTNode|None, cursor:Cursor, project:Project):
    return cx_ast.TypeInfo(
        lexicalParent.path if lexicalParent != None else None,
        cursor.displayname,
        makeSourceLocation(cursor, project),
        cursor.is_definition(),
        cursor.is_abstract_record()
    ) # TODO add visibility specifier

@ASTFactory(CursorKind.FIELD_DECL)
def factory_FieldInfo(lexicalParent:cx_ast.TypeInfo, cursor:Cursor, project:Project):
    return cx_ast.FieldInfo(
        lexicalParent.path,
        cursor.displayname,
        makeSourceLocation(cursor, project),
        makeVisibility(cursor),
        _make_FullyQualifiedTypeName(cursor.type)
    )

@ASTFactory(CursorKind.CXX_BASE_SPECIFIER)
def factory_ParentInfo(lexicalParent:cx_ast.TypeInfo, cursor:Cursor, project:Project):
    return cx_ast.ParentInfo(
        lexicalParent.path,
        _make_FullyQualifiedName(cursor),
        makeSourceLocation(cursor, project),
        makeVisibility(cursor),
        isExplicitVirtual(cursor)
    )

@ASTFactory(CursorKind.FRIEND_DECL)
def factory_FriendInfo(lexicalParent:cx_ast.TypeInfo, cursor:Cursor, project:Project):
    return cx_ast.FriendInfo(
        lexicalParent.path,
        _make_FullyQualifiedName( next(ClangParseContext._getChildren(cursor)) ),
        makeSourceLocation(cursor, project),
        makeVisibility(cursor)
    )

@ASTFactory(CursorKind.CONSTRUCTOR)
def factory_ConstructorInfo(lexicalParent:cx_ast.TypeInfo, cursor:Cursor, project:Project):
    return cx_ast.ConstructorInfo(
        lexicalParent.path,
        makeSourceLocation(cursor, project),
        cursor.is_definition(),
        cursor.is_deleted_method(),
        False, # TODO inline support
        makeVisibility(cursor)
    )

@ASTFactory(CursorKind.DESTRUCTOR)
def factory_DestructorInfo(lexicalParent:cx_ast.TypeInfo, cursor:Cursor, project:Project):
    return cx_ast.DestructorInfo(
        lexicalParent.path,
        makeSourceLocation(cursor, project),
        cursor.is_definition(),
        makeVisibility(cursor),
        isExplicitVirtual(cursor),
        isExplicitOverride(cursor),
        cursor.is_deleted_method(),
        False # TODO inline support
    )

@ASTFactory(CursorKind.CXX_METHOD, CursorKind.FUNCTION_DECL, CursorKind.FUNCTION_TEMPLATE)
def factory_FuncInfo_MemberOrStatic(lexicalParent:cx_ast.TypeInfo|None, cursor:Cursor, project:Project):
    if isinstance(lexicalParent, cx_ast.TypeInfo):
        if not cursor.is_static_method():
            # Nonstatic member function
            return cx_ast.MemFuncInfo(
                lexicalParent.path,
                cursor.spelling,
                makeSourceLocation(cursor, project),
                cursor.is_definition(),
                makeVisibility(cursor),
                isExplicitVirtual(cursor),
                isExplicitOverride(cursor),
                _make_FullyQualifiedTypeName(cursor.result_type),
                cursor.is_deleted_method(),
                False, # TODO inline support
                cursor.is_const_method(),
                False # TODO volatile support
            )
        else:
            # Static member function
            return cx_ast.StaticFuncInfo(
                lexicalParent.path,
                cursor.spelling,
                makeSourceLocation(cursor, project),
                cursor.is_definition(),
                makeVisibility(cursor),
                _make_FullyQualifiedTypeName(cursor.result_type),
                cursor.is_deleted_method(),
                False # TODO inline support
            )
    else:
        # Nonmember static function
        return cx_ast.GlobalFuncInfo(
            ( (lexicalParent.path if lexicalParent != None else "")+"::"+cursor.spelling )[2::],
            makeSourceLocation(cursor, project),
            cursor.is_definition(),
            _make_FullyQualifiedTypeName(cursor.result_type),
            cursor.is_deleted_method(),
            False # TODO inline support
        )

@ASTFactory(CursorKind.PARM_DECL)
def factory_ParameterInfo(lexicalParent:cx_ast.Callable, cursor:Cursor, project:Project):
    out = cx_ast.Callable.Parameter(
        lexicalParent.path,
        cursor.displayname,
        makeSourceLocation(cursor, project),
        _make_FullyQualifiedTypeName(cursor.type)
    )
    lexicalParent.parameters.append(out)
    out.astParent = lexicalParent
    return out

@ASTFactory(CursorKind.VAR_DECL)
def factory_GlobalVarInfo(lexicalParent:cx_ast.ASTNode|None, cursor:Cursor, project:Project):
    # TODO implement
    return None

@ASTFactory(CursorKind.ANNOTATE_ATTR)
def factory_Annotation(lexicalParent:cx_ast.ASTNode|None, cursor:Cursor, project:Project):
    return cx_ast.Annotation(
        lexicalParent.path,
        cursor.displayname,
        makeSourceLocation(cursor, project)
    )



# TODO probably slow, profile and rewrite

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

def _make_FullyQualifiedTypeName(ty:Type):
    return _typeGetAbsName(ty, False)

def _make_FullyQualifiedName(cursor:Cursor):
    return _getAbsName(cursor)

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



if __name__ == "__main__":
    import sys

    timings.switchTask(timings.TASK_ID_INIT)
    arg_parser = argparse.ArgumentParser(
        prog=os.path.basename(__file__),
        description="STIX Clang reader: Emitting and caching ASTs as a build step"
    )
    ClangParseContext.argparser_add_defaults(arg_parser)
    args = arg_parser.parse_args(sys.argv[1:])

    config.logger.info("Discovering files")
    timings.switchTask(timings.TASK_ID_DISCOVER)
    cx_project = Project(
        cx_ast_tooling.reduce_lists(args.targets),
        cx_ast_tooling.reduce_lists(args.includes)
    )
    cx_parser = ClangParseContext(cx_project, args, config.batch_stable_hash(["cx_ast_clang_reader.py"]))
    
    config.logger.info("Loading cache")
    cx_parser.configure()
    cx_parser.loadPrevOutput()
    config.logger.user(str(cx_parser.diff))
    
    config.logger.user("Parsing")
    timings.switchTask(timings.TASK_ID_WALK_AST_INTERNAL)
    cx_parser.ingest()
    
    config.logger.info("Finalizing")
    timings.switchTask(timings.TASK_ID_FINALIZE)
    cx_parser.saveOutput()
