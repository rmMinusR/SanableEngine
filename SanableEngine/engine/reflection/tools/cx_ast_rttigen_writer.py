import cx_ast
import cx_ast_tooling
import timings
import argparse
import os.path


class RttiGenerator(cx_ast_tooling.ASTConsumer):
    @staticmethod
    def argparser_add_defaults(parser: argparse.ArgumentParser):
        cx_ast_tooling.ASTConsumer.argparser_add_defaults(parser)
        parser.add_argument("--template-file", dest="template_file", default=None)
        parser.add_argument("--default-image-capture-backend", dest="default_image_capture_backend", choices=["cdo", "disassembly"], default="disassembly")
        parser.add_argument("--default-image-capture-status", dest="default_image_capture_status", choices=["enabled", "disabled"], default="enabled")
        
    def __init__(this, module:cx_ast.Module, args:argparse.Namespace):
        super().__init__(module, args)

    def execute(this):
        pass
    

if __name__ == "__main__":
    timings.switchTask(timings.TASK_ID_INIT)
    parser = cx_ast_tooling.default_argument_parser(
        prog=os.path.basename(__file__),
        description="STIX generation tool: Embedding RTTI in a C++ binary as a build step"
    )
