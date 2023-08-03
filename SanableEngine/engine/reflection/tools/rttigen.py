import read_ast
import cpp_concepts
import os.path
import argparse



########################## argparse setup ##########################

parser = argparse.ArgumentParser(
        prog=os.path.basename(__file__),
        description="Sanable Engine RTTI generation tool: Embedding RTTI in a C++ binary as a build step"
    )
parser.add_argument("--target", help="Folder containing entire TU, which must also include headers. Includes outside of this folder will not have RTTI generated.", required=True)
#parser_output = parser.add_mutually_exclusive_group()
#parser_output.add_argument("--embed"   , action="store_const", const="embed"   , dest="format", help="Output C++ to be embedded in binary. Automatic path.")
#parser_output.add_argument("--separate", action="store_const", const="separate", dest="format", help="Output a separate (non-binary) file that can be loaded at runtime. Automatic path.")
#parser_output.add_argument("--format", choices=["embed", "separate"], help="Specify an output format. Automatic path.")
#parser.add_argument("-o", "--output", help="Specify an output path. If format is not set, detect from file extension.")
parser.add_argument("-I", "--includes", help="Headers to scan, both from this target and its dependencies. Semicolon-separated list.")
parser.add_argument("-D", "--defines", help="Preprocessor definition. Semicolon-separated list.")
parser.add_argument("-o", "--output", help="Specify an output folder/file. Default: target/src/rtti.generated.cpp", default=None)
parser.add_argument("--", dest="compilerArgs", nargs='*', help="Arguments passed through to the compiler, such as includes and defines")

args = parser.parse_args()



########################## Argument processing and validation ##########################

#Process CMake-style lists
if args.includes != None:
    args.includes = args.includes.split(";")
else:
    args.includes = []

if args.defines != None:
    args.defines = args.defines.split(";")
else:
    args.defines = []

#Default output folder, if none specified
if args.output == None:
    args.output = os.path.join(args.target, "src")

#Default file name within folder, if a folder was specified
if os.path.isdir(args.output) or '.' not in os.path.basename(args.output):
    args.output = os.path.join(args.output, "rtti.generated.cpp")



########################## Main business logic ##########################

print("Parsing AST...")

targetModule = cpp_concepts.Module()
for i in read_ast.parseAuto(args.target):
    targetModule.parse(i)

scriptDir = os.path.dirname(os.path.abspath(__file__))
with open(os.path.join(scriptDir, "rtti.template.cpp"), "r") as f:
    template = "".join(f.readlines())

print("Rendering...")

def shortestRelPath(absPath):
    global args
    return min(
        [os.path.relpath(absPath, os.path.join(args.target, i)) for i in args.includes],
        key=len
    )

generated = template.replace("GENERATED_RTTI", targetModule.renderBody()) \
                    .replace("INCLUDE_DEPENDENCIES", "\n".join([ \
                         f'#include "{shortestRelPath(i)}"' for i in targetModule.renderIncludes() \
                    ]))

print(f"Writing to {args.output}...")

with open(args.output, "wt") as f:
    f.write(generated)

print("Done!")
