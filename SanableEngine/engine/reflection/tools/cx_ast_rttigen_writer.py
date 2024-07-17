from functools import cached_property
import cx_ast
import cx_ast_tooling
import timings
import argparse
import os.path
import config
import source_discovery


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
        this.args_parser:str = args.parser
        this.args_template:str = args.template_file
        this.default_image_capture_backend:str = args.default_image_capture_backend
        this.default_image_capture_status :str = args.default_image_capture_status

    def execute(this):
        config.logger.user(f"Rendering binders")

        with open(this.args_template) as templateFile: template = templateFile.read()
        
        generated = template \
                    .replace("GENERATED_RTTI", this.__renderBody()) \
                    .replace("PUBLIC_CAST_DECLS", this.__renderPreDecls()) \
                    .replace("INCLUDE_DEPENDENCIES", this.__renderIncludes())
        
        config.logger.user(f"Writing to {this.args_output}")


    @cached_property
    def __stableTypes(this):
        types:list[cx_ast.TypeInfo] = this.input.module.byType[cx_ast.TypeInfo]
        types = [i for i in types if i.location.file in this.input.project.files] # Filter: only our files
        types.sort(lambda i: i.path)
        return types


    def __renderPreDecls(this):
        out = []
        for ty in this.__stableTypes:
            # TODO

        return out
    
    def __renderBody(this):
        pass # TODO
    
    def __renderIncludes(this):
        includes:set[source_discovery.SourceFile] = set()
        for ty in this.__stableTypes:
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
    