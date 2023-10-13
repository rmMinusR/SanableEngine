########################## argparse setup ##########################

import argparse
import os.path

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

#Process CMake-style lists
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

#Default template file, if none specified
if args.template_file == None:
    scriptDir = os.path.dirname(os.path.abspath(__file__))
    args.template_file = os.path.join(scriptDir, "rtti.template.cpp")

#Default output folder, if none specified
if args.output == None:
    args.output = os.path.join(args.targets[0], "src")

#Default file name within folder, if a folder was specified
if os.path.isdir(args.output) or '.' not in os.path.basename(args.output):
    args.output = os.path.join(args.output, "rtti.generated.cpp")



########################## Main business logic ##########################

import source_discovery
source_discovery.additionalCompilerOptions = compilerArgs

config.logger.info("Discovering files")
projectFiles = source_discovery.discoverAll(args.targets)
sourceFiles = [f for f in projectFiles if f.type != None and not f.hasError]
for sourceFile in sourceFiles:
    sourceFile.additionalIncludes += args.includes

config.logger.log(100, "Parsing...")
import cpp_concepts
if args.cache != None: targetModule = cpp_concepts.Module.load(args.cache)
else: targetModule = cpp_concepts.Module()
targetModule.parseTU(sourceFiles)

config.logger.info("Finalizing...")
targetModule.finalize()
if args.cache != None: targetModule.save(args.cache)

config.logger.log(100, f"Rendering to {args.output}")

with open(args.template_file, "r") as f:
    template = "".join(f.readlines())

def shortestRelPath(absPath):
    global args
    return min(
        [min(
            [os.path.relpath(absPath, os.path.join(target, i)) for i in args.includes],
            key=len
        ) for target in args.targets],
        key=len
    )

generated = template.replace("GENERATED_RTTI", targetModule.renderBody()) \
                    .replace("INCLUDE_DEPENDENCIES", "\n".join(set([ \
                         f'#include "{shortestRelPath(i)}"' for i in targetModule.renderIncludes() \
                    ])))

config.logger.info(f"Writing to {args.output}")

with open(args.output, "wt") as f:
    f.write(generated)

config.logger.info("Done!")
