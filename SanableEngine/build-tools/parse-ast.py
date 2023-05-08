import clang.cindex
import sys, os

def parseAuto(target: str) -> list[clang.cindex.Cursor]:
	if os.path.isdir(target):
		return parseDir(target)
	elif os.path.splitext(target)[1] in [".h", ".hpp", ".inl", ".c", ".cpp"]:
		return parseFile(target)
	else:
		return []

def parseDir(target: str) -> list[clang.cindex.Cursor]:
	out = []
	for subpath in os.listdir(target):
		out += parseAuto(os.path.join(target, subpath))
	return out

def parseFile(target: str) -> list[clang.cindex.Cursor]:
	index = clang.cindex.Index.create()

	try:
		tu = index.parse(target, args=["-std=c++17", "--language=c++-header"], options=clang.cindex.TranslationUnit.PARSE_SKIP_FUNCTION_BODIES)
		return [i for i in tu.cursor.get_children() if isOurs(target, i)]
	except Exception as e:
		print(f"Error parsing translation unit for target {target}")
		raise e

def isOurs(sourceDir: str, i: clang.cindex.Cursor):
	return sourceDir == i.location.file.name and i.is_definition()

def isClass(cursor: clang.cindex.Cursor) -> bool:
	return i.kind in [clang.cindex.CursorKind.CLASS_DECL, clang.cindex.CursorKind.CLASS_TEMPLATE, clang.cindex.CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION]





if __name__ == "__main__":
	def _testRig(filePath):
		print(f"Parsing {filePath}:")
		for i in parseAuto(filePath):
			print(f" - {i.displayname} : {i.kind}")
		print("")

	_testRig(r"UserPlugin/")
	_testRig(r"PrimitivesPlugin/")
	_testRig(r"engine/memory/public/RawMemoryPool.hpp")
	_testRig(r"engine/")
