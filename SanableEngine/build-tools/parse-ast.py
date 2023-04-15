import clang.cindex
import sys, os

#target = sys.argv[1]
target = r"D:\_Data\source\repos\SanableEngine\SanableEngine\engine\test"

index = clang.cindex.Index.create()
print("Working dir: "+os.getcwd())
print("Parsing: "+target)
tu = index.parse(target, args="-std=c++17")
print(tu)
