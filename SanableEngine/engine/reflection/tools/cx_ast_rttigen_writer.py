from functools import cached_property
from textwrap import indent
import cx_ast
import cx_ast_tooling
import timings
import argparse
import os.path
import config
import source_discovery
import typing


class RttiGenerator(cx_ast_tooling.ASTConsumer):
    @staticmethod
    def argparser_add_defaults(parser: argparse.ArgumentParser):
        cx_ast_tooling.ASTConsumer.argparser_add_defaults(parser)
        parser.add_argument("-o", "--output", default=None, help="Specify an output folder/file. If not specified, defaults to target/src/rtti.generated.cpp")
        parser.add_argument("--template-file", dest="template_file", default=None)
        parser.add_argument("--default-image-capture-backend", dest="default_image_capture_backend", choices=["cdo", "disassembly"], default="disassembly")
        parser.add_argument("--default-image-capture-status", dest="default_image_capture_status", choices=["enabled", "disabled"], default="enabled")
        
    def __init__(this, args:argparse.Namespace):
        super().__init__(args)
        
        # Parse args
        this.args_output:str = args.output
        this.args_template:str = args.template_file
        this.default_image_capture_backend:str = args.default_image_capture_backend
        this.default_image_capture_status :str = args.default_image_capture_status

    def __isOurs(this, symbol:cx_ast.ASTNode|list[cx_ast.ASTNode]):
        if isinstance(symbol, cx_ast.ASTNode) and symbol.definitionLocation != None:
            return symbol.definitionLocation.file in this.input.project.files
        elif isinstance(symbol, list) and symbol[0].definitionLocation != None:
            return symbol[0].definitionLocation.file in this.input.project.files
        else:
            return False # Probably...

    def configure(this):
        super().configure()
        stableSymbols = this.input.module.contents.values()
        stableSymbols = [i for i in stableSymbols if this.__isOurs(i)] # Filter: only our files
        stableSymbols.sort(key=lambda i: i.path if isinstance(i, cx_ast.ASTNode) else i[0].path)
        this.stableSymbols = stableSymbols

    def execute(this):
        config.logger.user(f"Rendering binders")

        with open(this.args_template) as templateFile: template = templateFile.read()
        
        _rendered = this.__renderBody()
        renderedPreDecls = "\n"  .join(_rendered[0])
        renderedBody     = "\n\n".join(_rendered[1])

        generated = template \
                    .replace("GENERATED_RTTI", renderedBody) \
                    .replace("PUBLIC_CAST_DECLS", renderedPreDecls) \
                    .replace("INCLUDE_DEPENDENCIES", this.__renderIncludes())
        
        config.logger.user(f"Writing to {this.args_output}")
        with open(this.args_output, "wt") as outputFile: outputFile.write(generated)

    def __renderBody(this):
        forwardDecls = []
        bodyDecls = []
        
        # Render symbols
        for sym in this.stableSymbols:
            isOurs = sym.definitionLocation != None and sym.definitionLocation.file in this.input.project.files
            if isOurs and type(sym) in RttiGenerator.renderers.keys() and not isinstance(sym, cx_ast.Member):
                genFn = RttiGenerator.renderers[type(sym)]
                generated = genFn(sym)
                if generated != None:
                    forwardDecls.extend(generated[0])
                    bodyDecls   .extend(generated[1])
            else:
                bodyDecls.append(f"//Skipping forward-declared {type(sym)} missing definition: {sym.path}")

        return (forwardDecls, bodyDecls)
    
    rendererOutput_t = tuple[str,str]|None
    renderer_t = typing.Callable[[cx_ast.ASTNode], rendererOutput_t]
    renderers:dict[typing.Type, renderer_t] = dict() # Only applies to global symbols
    memRenderers:dict[typing.Type, renderer_t] = dict()

    def __renderIncludes(this):
        includes:set[source_discovery.SourceFile] = set()
        for ty in this.stableTypes:
            includes.add(ty.location.file)
        includes:list[source_discovery.SourceFile] = list(includes)
        includes.sort(lambda i: i.path)
        
        def shortestRelPath(sourceFile:source_discovery.SourceFile):
            def makeRelPath(parentDir:str):
                if sourceFile.abspath.startswith(parentDir):
                    return sourceFile.abspath[:-len(parentDir)]
                else:
                    return None

            candidates = [makeRelPath(i) for i in this.input.project.includeDirs]
            candidates = [i for i in candidates if i != None]
            
            return min(candidates, key=len)

        return "\n".join([ f'#include "{shortestRelPath(i)}"' for i in includes ])
    
def RttiRenderer(*types:typing.Type):
    """
    Helper to auto-register renderer functions
    """
    def registrar(func:RttiGenerator.renderer_t):
        for i in types:
            assert i not in RttiGenerator.renderers.keys()
            RttiGenerator.renderers[i] = func
        return func
    return registrar

def MemRttiRenderer(*types:typing.Type):
    """
    Helper to auto-register renderer functions
    """
    def registrar(func:RttiGenerator.renderer_t):
        for i in types:
            assert i not in RttiGenerator.memRenderers.keys()
            RttiGenerator.memRenderers[i] = func
        return func
    return registrar


@RttiRenderer(cx_ast.TypeInfo)
def render_Type(ty:cx_ast.TypeInfo):
    preDecls :list[str] = []
    bodyDecls:list[str] = []

    for mem in ty.children:
        if type(mem) in RttiGenerator.memRenderers.keys():
            genFn = RttiGenerator.memRenderers[type(mem)]
            memGenerated = genFn(mem)
            if memGenerated != None:
                preDecls .extend(memGenerated[0])
                bodyDecls.extend(memGenerated[1])

    return (
        "\n".join(preDecls),
        "\n".join(
            f"//{ty.path}",
            "{",
            indent("\n".join(bodyDecls), ' '*4),
            "}"
        )
    )

def makePubCastKey(obj:cx_ast.ASTNode):
    return "".join([
        (i if i.isalnum() or i in "_" else '_')
        for i in obj.path
    ])

@MemRttiRenderer(cx_ast.FieldInfo)
def render_field(field:cx_ast.FieldInfo):
    # Detect how to reference
    #if field.visibility != cx_ast.Member.Visibility.Public:
    pubCastKey = makePubCastKey(field)
    preDecl = f'PUBLIC_CAST_GIVE_FIELD_ACCESS({pubCastKey}, {field.astParent.path}, {field.ownName}, {field.typeName});'
    pubReference = f"DO_PUBLIC_CAST_OFFSETOF_LAMBDA({pubCastKey}, {field.astParent.path})"
    #else:
    #    preDecl = f"//{field.path} is already public: no need for public_cast"
    #    pubReference = f"[]({field.astParent.path})" + "{ " + f"return offsetof({field.path});" + " }"

    # TODO handle anonymous types (especially if private)
    body = f'builder.addField<{field.typeName}>("{field.ownName}", {pubReference});'
    
    return (body, pubReference)

@MemRttiRenderer(cx_ast.MemFuncInfo)
def render_memFunc(func:cx_ast.MemFuncInfo):
    # Detect how to reference
    #if func.visibility != cx_ast.Member.Visibility.Public:
    pubCastKey = makePubCastKey(func)
    formatter = {
        "key": pubCastKey,
        "TClass": func.astParent.path,
        "returnType": func.returnTypeName,
        "params": ", ".join([i.typeName for i in func.parameters]),
        "name": func.ownName,
        "this_maybe_const": " const" if func.isConstMethod else ""
    }
    preDecl = "\n".join([
        'PUBLIC_CAST_DECLARE_KEY_BARE({key});'.format_map(formatter),
		'template<> struct ::public_cast::_type_lut<PUBLIC_CAST_KEY_OF({key})>'.format_map(formatter) + ' { ' + 'using ptr_t = {returnType} ({TClass}::*)({params}){this_maybe_const};'.format_map(formatter) + ' };',
		'PUBLIC_CAST_GIVE_ACCESS_BARE({key}, {TClass}, {name});'.format_map(formatter)
    ])
    pubReference = f"DO_PUBLIC_CAST({pubCastKey})"
    #else:
    #    preDecl = f"//{func.path} is already public: no need for public_cast"
    #    pubReference = func.path
    
    if not func.isDeleted:
        paramNames = [i.displayName for i in func.parameters] # TODO implement name capture on C++ side
        body = f"builder.addMemberFunction(stix::MemberFunction::make({pubReference}), \"{func.ownName}\", {func.visibility}, {str(func.isVirtual).lower()});"
    else:
        body = f"//Cannot capture deleted function {func.path}"
        # TODO handle template funcs

    return (preDecl, body)



if __name__ == "__main__":
    import sys

    timings.switchTask(timings.TASK_ID_INIT)
    parser = argparse.ArgumentParser(
        prog=os.path.basename(__file__),
        description="STIX generation tool: Embedding RTTI in a C++ binary as a build step"
    )
    RttiGenerator.argparser_add_defaults(parser)
    args = parser.parse_args(sys.argv[1:])

    rttigen = RttiGenerator(args)
    rttigen.configure()
    rttigen.execute()
    