import cx_ast
from source_discovery import *
import config
import timings

from collections.abc import Iterator
import typing


class ClangParseContext:
    def __init__(this, module:cx_ast.Module|None, diff:ProjectDiff|None, project:Project):
        if diff == None: diff = ProjectDiff(None, project)
        if module == None: module = cx_ast.Module()
        this.module = module
        this.diff = diff
        this.project = project

    def ingest(this):
        #diff = ProjectDiff(prev_project, live_project)
        #config.logger.log(config.LOG_USER_LEVEL, str(diff))
        
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
                if this.__isOurs(cursor): this.__ingestCursor(None, cursor)
                
        this.module.linkAll()


    @staticmethod
    def __getChildren(cursor: Cursor) -> Iterator[Cursor]:
        return timings.timeScoped(lambda: cursor.get_children(), timings.TASK_ID_WALK_AST_EXTERNAL)


    def __isOurs(this, cursor: Cursor):
        cursorFilePath = cursor.location.file.name.replace(os.altsep, os.sep)
        return any((cursorFilePath == i.path for i in this.project.files))


    factories:dict[CursorKind, typing.Callable[[cx_ast.ASTNode|None, Cursor, Project], cx_ast.ASTNode|None]] = dict()

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
            


def factory_TypeInfo(parent:cx_ast.ASTNode|None, cursor:Cursor, project:Project):
    loc:SourceLocation = cursor.location
    sourceFile = project.getFile(loc.file.name)
    assert sourceFile != None
    return cx_ast.TypeInfo(
        parent.path if parent != None else None,
        cursor.displayname,
        cx_ast.SourceLocation(sourceFile, loc.line, loc.column),
        cursor.is_abstract_record()
    )

for i in [
            CursorKind.CLASS_DECL, CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION,
			CursorKind.STRUCT_DECL, CursorKind.UNION_DECL
            # TODO Removed temporarily: CursorKind.CLASS_TEMPLATE
        ]: ClangParseContext.factories[i] = factory_TypeInfo
