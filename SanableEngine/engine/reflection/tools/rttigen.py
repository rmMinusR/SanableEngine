########################## argparse setup ##########################

import argparse
import os.path
import timings

timings.switchTask(timings.TASK_ID_INIT)

parser = argparse.ArgumentParser(
        prog=os.path.basename(__file__),
        description="Sanable Engine RTTI generation tool: Embedding RTTI in a C++ binary as a build step"
    )
parser.add_argument("--targets", help="Semicolon list of folders containing entire TU, which must also include headers. Includes not listed will be exluded from RTTI generation.", required=True)
parser.add_argument("-I", "--include", dest="includes", help="Headers to scan, both from this target and its dependencies. Semicolon-separated list.")
parser.add_argument("-D", "--define" , dest="defines" , help="Preprocessor definition. Semicolon-separated list.")
parser.add_argument("-o", "--output", default=None, help="Specify an output folder/file. If not specified, defaults to target/src/rtti.generated.cpp")
parser.add_argument("--cache", default=None, help="If specified, cache results for future runs. Helps improve speed.")
parser.add_argument("-v", "--verbose", action="store_true")
parser.add_argument("--template-file", dest="template_file", default=None)
parser.add_argument("--default-image-capture-backend", dest="default_image_capture_backend", choices=["cdo", "disassembly"], default="disassembly")
parser.add_argument("--default-image-capture-status", dest="default_image_capture_status", choices=["enabled", "disabled"], default="enabled")
parser.add_argument("--task-profiling", dest="task_profiling", action="store_true", help="Print per-task profiling")
parser.add_argument("--call-profiling", dest="call_profiling", action="store_true", help="Enable function call profiling")
parser.add_argument("--", dest="compilerArgs", nargs='*', help="Additional arguments passed through to the compiler. May include already-listed defines and includes.")

# Filter out arguments passed through to the compiler
import sys
if "--" in sys.argv:
    compilerArgsSplitIndex = sys.argv.index("--")
    compilerArgs = sys.argv[compilerArgsSplitIndex+1:]
    ownArgsList = sys.argv[1:compilerArgsSplitIndex]
else:
    compilerArgsSplitIndex = -1
    compilerArgs = []
    ownArgsList = sys.argv[1:]

# Parse arguments directed at RTTI generation
args = parser.parse_args(ownArgsList)



################### Argument processing and validation ###################

import config

if args.verbose:
    config.logger.setLevel(0)

# Process CMake-style lists
args.targets = [i for i in args.targets.split(";") if len(i) != 0]
assert len(args.targets) > 0, "Must provide at least one target!"

if args.includes != None:
    args.includes = args.includes.split(";")
else:
    args.includes = []

if args.defines != None:
    args.defines = args.defines.split(";")
else:
    args.defines = []

# Default template file, if none specified
if args.template_file == None:
    scriptDir = os.path.dirname(os.path.abspath(__file__))
    args.template_file = os.path.join(scriptDir, "rtti.template.cpp")

# Default output folder, if none specified
if args.output == None:
    args.output = os.path.join(args.targets[0], "src")

# Default file name within folder, if a folder was specified
if os.path.isdir(args.output) or '.' not in os.path.basename(args.output):
    args.output = os.path.join(args.output, "rtti.generated.cpp")

# Default image capture config
import cpp_concepts
cpp_concepts.defaultImageCaptureStatus = args.default_image_capture_status
cpp_concepts.defaultImageBackend = args.default_image_capture_backend

def main_business_logic():
    ########################## Main business logic ##########################

    import source_discovery
    timings.switchTask(timings.TASK_ID_DISCOVER)

    config.logger.info("Discovering files")
    project = source_discovery.Project(args.targets, args.includes)
    project.additionalCompilerOptions = compilerArgs
    template = source_discovery.SourceFile(args.template_file, project)

    # Attempt to load from cache, if present
    # CACHE FORMAT: version hash, previous module, previous project, previous template
    targetModule = cpp_concepts.Module(
        defaultImageCaptureStatus=args.default_image_capture_status,
        defaultImageCaptureBackend=args.default_image_capture_backend
    )
    isTemplateDirty:bool = True
    prevProject = None
    import pickle
    if args.cache != None and os.path.exists(args.cache):
        # Load
        with open(args.cache, "rb") as file: cacheFileRepr = file.read()
        try: cacheRaw = pickle.loads(cacheFileRepr)
        except EOFError: cacheRaw = None # We have a blank file

        # Check
        if cacheRaw != None:
            try:
                cacheVersion = cacheRaw[0]
                if cacheVersion != config.version_hash:
                    config.logger.info("Detected changes to RTTI generator script. Discarding cache.")
                else:
                    (_, prevModule, prevProject, prevTemplateHash) = cacheRaw
                    if not targetModule.configMatches(prevModule):
                        config.logger.info("Project configuration has changed. Discarding cache.")
                    else:
                        isTemplateDirty = (prevTemplateHash == template.contentsHash)
                        targetModule = prevModule
                        config.logger.info(f"Loaded {len(targetModule.symbols)} symbols from cache")
            except Exception as e:
                config.logger.error(f"Encountered the following error while loading cache. Cache will be discarded and regenerated.")
                config.logger.error(f"{e}")

    config.logger.user("Parsing...")
    filesChanged = targetModule.parseTU(prevProject, project)

    # Finalize and save back to cache
    timings.switchTask(timings.TASK_ID_FINALIZE)
    config.logger.info("Finalizing...")
    targetModule.finalize()
    if args.cache != None:
        data = (config.version_hash, targetModule, project, template.contentsHash)
        with open(args.cache, "wb") as file: file.write(pickle.dumps( data ))

    config.logger.user(f"Rendering to {args.output}")

    def shortestRelPath(absPath):
        global args
        def tryMakeRelPath(p):
            try: return os.path.relpath(absPath, p)
            except: return p
        return min(
            [min(
                [tryMakeRelPath(os.path.join(target, i)) for i in args.includes],
                key=len
            ) for target in args.targets],
            key=len
        )

    generated = template.contents \
                        .replace("GENERATED_RTTI", targetModule.renderBody()) \
                        .replace("PUBLIC_CAST_DECLS", targetModule.renderPreDecls()) \
                        .replace("INCLUDE_DEPENDENCIES", "\n".join([ \
                             f'#include "{shortestRelPath(i)}"' for i in targetModule.renderIncludes() \
                        ]))

    # Skip write if there's nothing to write
    shouldWrite = True
    if os.path.exists(args.output):
        with open(args.output, "r") as f:
            prevGenerated = "".join(f.readlines())
            shouldWrite = (prevGenerated != generated)


    if shouldWrite:
        with open(args.output, "wt") as f:
            f.write(generated)
    else:
        config.logger.info("Skipping write: nothing to do")

    config.logger.info("Done!")


def print_timings():
    config.logger.info("")
    config.logger.info("Task timings:")
    for k in timings.timings.keys():
        config.logger.info(f" - {k}: {timings.timings[k]}")


# Invoke function profile, or just normal invoke
if args.call_profiling:
    import profile
    from pstats import SortKey, Stats
    profiler = profile.Profile()
    profiler.runcall(main_business_logic)
    
    if args.task_profiling: print_timings()

    config.logger.info("")
    config.logger.info("Function timings:")
    Stats(profiler).strip_dirs().sort_stats(SortKey.CUMULATIVE).print_stats()
else:
    main_business_logic()
    
    if args.task_profiling: print_timings()
