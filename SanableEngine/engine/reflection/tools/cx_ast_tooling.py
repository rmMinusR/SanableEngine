import cx_ast
import source_discovery
import abc
import argparse
import os.path
import config
import pickle



def reduce_lists(args:list[str], delim=';'):
    if isinstance(args, str): return [args]
    out:list[str] = []
    for i in args: out.extend([element for element in i.split(delim) if len(element)!=0])
    return out



class SavedAST:
    def __init__(this, project:source_discovery.Project, module:cx_ast.Module, userData=None):
        this.project = project
        this.module = module
        this.userData = userData

    @staticmethod
    def load(file, requestedVersion):
        try: cacheRaw = pickle.loads(file.read())
        except EOFError: return None # We have a blank file
        
        cacheVersion = cacheRaw[0]
        if cacheVersion != requestedVersion: return None # We have an outdated cache
        
        (_, prevModule, prevProject, userData) = cacheRaw
        return SavedAST(prevProject, prevModule, userData)
    
    def save(this, file, programVersion):
        data = (programVersion, this.module, this.project, this.userData)
        file.write(pickle.dumps( data ))



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
        

    def __init__(this, project:source_discovery.Project, args:argparse.Namespace):
        this.project = project
        
        # Vars loaded or derived from cache
        this.module = cx_ast.Module()
        this.diff   = source_discovery.ProjectDiff(None, this.project)

        # Process args

        this.debug_task_profiling:bool = args.task_profiling
        this.debug_call_profiling:bool = args.call_profiling
        this.debug_verbose:bool = args.verbose

        # Process CMake-style lists
        this.args_targets  = reduce_lists(args.targets)
        this.args_includes = reduce_lists(args.includes) if args.includes != None else []
        this.args_defines  = reduce_lists(args.defines ) if args.defines  != None else []
        assert len(args.targets) > 0, "Must provide at least one target!"
        
        # Default output folder, if none specified
        this.output:str = args.output if args.output != None else os.path.join(args.targets[0], "src")

        # Default file name within folder, if a folder was specified
        if os.path.isdir(this.output) or '.' not in os.path.basename(this.output):
             this.output = os.path.join(args.output, "ast.stix")
        else:
             this.output:str = args.output


    def configure(this):
        if this.debug_verbose:
            config.logger.setLevel(0)

    @abc.abstractmethod
    def ingest(this): pass
    
    
    def loadPrevOutput(this):
        try:
            
            with open(this.output, 'rb') as file: oldOutput = SavedAST.load(file, config.version_hash)
        
            if oldOutput == None:
                config.logger.info("Detected changes to RTTI generator script. Entire translation unit will be regenerated.")
            else:
                this.diff   = source_discovery.ProjectDiff(oldOutput.project, this.project)
                this.module = oldOutput.module
        
        except Exception as e:
            config.logger.error("Encountered the following error while loading cache. Cache will be discarded and regenerated.")
            config.logger.error(e)
            
    def saveOutput(this):
        with open(this.output, 'wb') as file: SavedAST(this.project, this.module).save(file, config.version_hash)



class ASTConsumer:
    """
    Takes an AST and emits some other files
    """

    __metaclass__ = abc.ABCMeta
    
    @staticmethod
    def argparser_add_defaults(parser:argparse.ArgumentParser):
        parser.add_argument("-i", "--input", dest="input", help="AST file produced by a STIX parser")
        parser.add_argument("-v", "--verbose", action="store_true")
        

    def __init__(this, args:argparse.Namespace):
        # Process args
        this.args_input  :str = args.input
        this.args_verbose:bool = args.verbose
    

    def configure(this):
        if this.args_verbose:
            config.logger.setLevel(0)
            
        this.input = SavedAST.load(this.args_input, config.version_hash)


    def execute(this): pass
