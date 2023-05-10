import read_ast
import cpp_concepts

m = cpp_concepts.Module()
for i in read_ast.parseAuto(r"UserPlugin/"):
    m.parse(i)

print()
print()
print(m.render())
print()
print()
