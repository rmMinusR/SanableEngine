import read_ast
import cpp_concepts
import os.path

target = r"engine/memory/"

m = cpp_concepts.Module()
for i in read_ast.parseAuto(target):
    m.parse(i)

if False:
    f = open(os.path.join(target, "src", "rtti.generated.cpp"), "wt")
    f.writelines([
        '#include "TypeInfo.hpp"\n',
        '#include "FieldInfo.hpp"\n',
        '#include "Module.hpp"\n',
        '\n'
    ])
    f.write(m.render())
    f.close()
else:
    print()
    print()
    print(m.render())
    print()
    print()
