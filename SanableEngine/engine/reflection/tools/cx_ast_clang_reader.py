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

    def __init__(this, project:Project, args:argparse.Namespace):
        super().__init__(project, args)

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
            for cursor in ClangParseContext.__getChildren(source.parse()):
                # Only capture what's in the current file
                cursorFilePath = cursor.location.file.name.replace(os.altsep, os.sep)
                if source.path == cursorFilePath: this.__ingestCursor(None, cursor)
                
        this.module.linkAll()


    @staticmethod
    def __getChildren(cursor: Cursor) -> Iterator[Cursor]:
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
        if kind in ClangParseContext.factories.keys():
            result = ClangParseContext.factories[kind](parent, cursor, this.project)
            if result != None:
                this.module.register(result)
                for child in ClangParseContext.__getChildren(cursor): this.__ingestCursor(result, child)
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
    return any([i.kind == CursorKind.CXX_OVERRIDE_ATTR for i in ClangParseContext.__getChildren(cursor)])



@ASTFactory(
    CursorKind.CLASS_DECL, CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION,
	CursorKind.STRUCT_DECL, CursorKind.UNION_DECL
    # TODO Removed temporarily: CursorKind.CLASS_TEMPLATE
)
def factory_TypeInfo(lexicalParent:cx_ast.ASTNode|None, cursor:Cursor, project:Project):
    if not cursor.is_definition(): return None
    return cx_ast.TypeInfo(
        lexicalParent.path if lexicalParent != None else None,
        cursor.displayname,
        makeSourceLocation(cursor, project),
        cursor.is_abstract_record()
    ) # TODO add visibility specifier

@ASTFactory(CursorKind.FIELD_DECL)
def factory_FieldInfo(lexicalParent:cx_ast.TypeInfo, cursor:Cursor, project:Project):
    if cursor.is_definition(): return None # Discard definition, all we care about is the declaration in the class braces
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
        _make_FullyQualifiedName( next(ClangParseContext.__getChildren(cursor)) ),
        makeSourceLocation(cursor, project),
        makeVisibility(cursor)
    )

@ASTFactory(CursorKind.CONSTRUCTOR)
def factory_ConstructorInfo(lexicalParent:cx_ast.TypeInfo, cursor:Cursor, project:Project):
    if cursor.is_definition() and not isinstance(lexicalParent, cx_ast.TypeInfo): return None # Discard out-of-line definitions, but keep inline definitions and declarations
    return cx_ast.ConstructorInfo(
        lexicalParent.path,
        makeSourceLocation(cursor, project),
        cursor.is_deleted_method(),
        makeVisibility(cursor)
    )

@ASTFactory(CursorKind.DESTRUCTOR)
def factory_DestructorInfo(lexicalParent:cx_ast.TypeInfo, cursor:Cursor, project:Project):
    if cursor.is_definition() and not isinstance(lexicalParent, cx_ast.TypeInfo): return None # Discard out-of-line definitions, but keep inline definitions and declarations
    return cx_ast.DestructorInfo(
        lexicalParent.path,
        makeSourceLocation(cursor, project),
        cursor.is_deleted_method(),
        makeVisibility(cursor),
        isExplicitVirtual(cursor),
        isExplicitOverride(cursor)
    )

@ASTFactory(CursorKind.CXX_METHOD, CursorKind.FUNCTION_DECL, CursorKind.FUNCTION_TEMPLATE)
def factory_FuncInfo_MemberOrStatic(lexicalParent:cx_ast.TypeInfo|None, cursor:Cursor, project:Project):
    if isinstance(lexicalParent, cx_ast.TypeInfo) and not cursor.is_static_method():
        # Nonstatic member function
        return cx_ast.MemFuncInfo(
            lexicalParent.path,
            # TODO: where did we inherit from?
            cursor.displayname,
            makeSourceLocation(cursor, project),
            makeVisibility(cursor),
            isExplicitVirtual(cursor),
            isExplicitOverride(cursor),
            _make_FullyQualifiedTypeName(cursor.result_type),
            cursor.is_deleted_method()
        )
    else:
        # Nonmember or static member function
        # TODO implement, and ensure only one copy makes it through
        return None

@ASTFactory(CursorKind.PARM_DECL)
def factory_ParameterInfo(lexicalParent:cx_ast.Callable, cursor:Cursor, project:Project):
    return cx_ast.Callable.Parameter(
        lexicalParent.path,
        cursor.displayname,
        makeSourceLocation(cursor, project),
        _make_FullyQualifiedTypeName(cursor.type)
    )

@ASTFactory(CursorKind.VAR_DECL)
def factory_GlobalVarInfo(lexicalParent:cx_ast.ASTNode|None, cursor:Cursor, project:Project):
    # TODO implement
    return None


if __name__ == "__main__":
    import sys

    timings.switchTask(timings.TASK_ID_INIT)
    arg_parser = cx_ast_tooling.default_argument_parser(
        prog=os.path.basename(__file__),
        description="STIX Clang reader: Emitting and caching ASTs as a build step"
    )
    args = arg_parser.parse_args(sys.argv[1:])

    config.logger.info("Discovering files")
    timings.switchTask(timings.TASK_ID_DISCOVER)
    cx_project = Project(args.targets, args.includes)
    cx_parser = ClangParseContext(cx_project, args)
    
    config.logger.info("Loading cache")
    cx_parser.configure()
    cx_parser.loadPrevOutput()
    config.logger.user(str(cx_parser.diff))
    
    config.logger.user("Parsing.")
    timings.switchTask(timings.TASK_ID_WALK_AST_INTERNAL)
    cx_parser.ingest()
    
    config.logger.info("Finalizing")
    timings.switchTask(timings.TASK_ID_FINALIZE)
    cx_parser.saveOutput()
