import cx_ast
import source_discovery
import abc
import argparse
import os.path


class ASTParser:
    """
    Reads source tree(s) and emits an AST
    """

    __metaclass__ = abc.ABCMeta
    
    @staticmethod
    def argparser_add_defaults(parser:argparse.ArgumentParser):
        parser.add_argument("--targets", help="Semicolon list of folders containing entire TU, which must also include headers. Includes not listed will be exluded from RTTI generation.", required=True)
        parser.add_argument("-I", "--include", dest="includes", help="Headers to scan, both from this target and its dependencies. Semicolon-separated list.")
        parser.add_argument("-D", "--define" , dest="defines" , help="Preprocessor definition. Semicolon-separated list.")
        parser.add_argument("-o", "--output", required=True, help="Specify where to save AST to")
        parser.add_argument("-v", "--verbose", action="store_true")
        parser.add_argument("--task-profiling", dest="task_profiling", action="store_true", help="Print per-task profiling")
        parser.add_argument("--call-profiling", dest="call_profiling", action="store_true", help="Enable function call profiling")
        
    def __init__(this, module:cx_ast.Module|None, diff:source_discovery.ProjectDiff|None, project:source_discovery.Project, args:argparse.Namespace):
        if diff == None: diff = source_discovery.ProjectDiff(None, project)
        if module == None: module = cx_ast.Module()
        this.module = module
        this.diff = diff
        this.project = project

        # Process args

        this.debug_task_profiling:bool = args.task_profiling
        this.debug_call_profiling:bool = args.call_profiling

        # Process CMake-style lists
        this.args_targets :list[str] = [i for i in args.targets.split(";") if len(i) != 0]
        this.args_includes:list[str] = args.includes.split(";") if args.includes != None else []
        this.args_defines :list[str] = args.defines .split(";") if args.defines  != None else []
        assert len(args.targets) > 0, "Must provide at least one target!"
        
        # Default output folder, if none specified
        this.args_output = args.output if args.output != None else os.path.join(args.targets[0], "src")

        # Default file name within folder, if a folder was specified
        if os.path.isdir(args.output) or '.' not in os.path.basename(args.output):
             this.args_output = os.path.join(args.output, "ast.stix")
        else:
             this.args_output = args.output
        

    def ingest(this): pass    



class ASTConsumer:
    """
    Takes an AST and emits some other files
    """

    __metaclass__ = abc.ABCMeta
    
    @staticmethod
    def argparser_add_defaults(parser:argparse.ArgumentParser):
        parser.add_argument("-i", "--input", dest="input", help="AST file produced by a STIX parser")
        parser.add_argument("-o", "--output", default=None, help="Specify an output folder/file. If not specified, defaults to target/src/rtti.generated.cpp")
        parser.add_argument("-v", "--verbose", action="store_true")
        
    def __init__(this, module:cx_ast.Module, args:argparse.Namespace):
        this.module = module
    
    def execute(this): pass
