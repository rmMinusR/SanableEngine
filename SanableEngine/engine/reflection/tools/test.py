import read_ast
import cpp_concepts
import os.path

target = r"UserPlugin/"

m = cpp_concepts.Module()
for i in read_ast.parseAuto(target):
    m.parse(i)

scriptDir = os.path.dirname(os.path.abspath(__file__))
with open(os.path.join(scriptDir, "rtti.template.cpp"), "r") as f:
    template = "".join(f.readlines())

generated = template.replace("GENERATED_RTTI", m.render())

if True:
    generatedFilePath = os.path.join(target, "src", "rtti.generated.cpp")
    print("Writing to "+generatedFilePath)
    with open(generatedFilePath, "wt") as f:
        f.write(generated)
else:
    print()
    print("=============================================")
    print()
    print(generated)
    print()
    print("=============================================")
    print()
