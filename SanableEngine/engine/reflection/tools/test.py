import read_ast
import cpp_concepts
import os.path

targetModulePath = r"UserPlugin/"
includes = ["public", "private", "include", "."]

generatedFilePath = os.path.join(targetModulePath, "src", "rtti.generated.cpp")

def shortestRelPath(absPath):
    global includes
    return min( \
        [os.path.relpath(absPath, os.path.join(targetModulePath, i)) for i in includes], \
        key=len \
    )

print("Parsing AST...")

targetModule = cpp_concepts.Module()
for i in read_ast.parseAuto(targetModulePath):
    targetModule.parse(i)

scriptDir = os.path.dirname(os.path.abspath(__file__))
with open(os.path.join(scriptDir, "rtti.template.cpp"), "r") as f:
    template = "".join(f.readlines())

print("Rendering...")

generated = template.replace("GENERATED_RTTI", targetModule.renderBody()) \
                    .replace("INCLUDE_DEPENDENCIES", "\n".join([ \
                         f'#include "{shortestRelPath(i)}"' for i in targetModule.renderIncludes() \
                    ]))

print(f"Writing to {generatedFilePath}...")

with open(generatedFilePath, "wt") as f:
    f.write(generated)

print("Done!")
