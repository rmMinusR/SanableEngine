import clang.cindex
import sys, os
from typing import Generator, Iterator
import typing

index = clang.cindex.Index.create()
fileTypes = {
	".h"   : "c++-header",
	".hpp" : "c++-header",
	".inl" : "c++-header",
	".c"   : "c++",
	".cpp" : "c++"
}

def parseAuto(target: str) -> Generator[clang.cindex.Cursor, None, None]:
	if os.path.isdir(target):
		return parseDir(target)

	ext = os.path.splitext(target)[1]
	if ext in fileTypes.keys():
		return parseFile(target)
	else:
		print(f"WARNING: Skipping file of unknown type \"{ext}\" ({target})")

def parseDir(target: str) -> Generator[clang.cindex.Cursor, None, None]:
	for subpath in os.listdir(target):
		it = parseAuto(os.path.join(target, subpath))
		if it != None:
			for i in it:
				yield i

def parseFile(target: str) -> Generator[clang.cindex.Cursor, None, None]:
	ext = os.path.splitext(target)[1]
	fileType = fileTypes[ext]
	try:
		tu = index.parse(target, args=["-std=c++17", "--language="+fileType])#, options=clang.cindex.TranslationUnit.PARSE_SKIP_FUNCTION_BODIES)

		for i in tu.cursor.get_children():
			if isOurs(target, i):
				yield i
		
	except Exception as e:
		print(f"Error parsing translation unit for target {target}")
		raise e

def isOurs(sourceDir: str, i: clang.cindex.Cursor):
	return sourceDir == i.location.file.name and i.is_definition()

def isClass(cursor: clang.cindex.Cursor) -> bool:
	return i.kind in [clang.cindex.CursorKind.CLASS_DECL, clang.cindex.CursorKind.CLASS_TEMPLATE, clang.cindex.CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION]



def flatten(target: Iterator | clang.cindex.Cursor) -> Generator[clang.cindex.Cursor, None, None]:
	# Default case: Cursor
	if isinstance(target, clang.cindex.Cursor):
		if target.kind == clang.cindex.CursorKind.NAMESPACE:
			for i in target.get_children():
				for j in flatten(i):
					yield j
		elif target.kind == clang.cindex.CursorKind.CLASS_DECL: # FIXME: Won't work for templates
			yield target
			for i in target.get_children():
				yield i
		else:
			yield target

	# Option for Cursor generators
	elif isinstance(target, (Iterator, Generator)):
		for i in target:
			for j in flatten(i):
				yield j

	else:
		raise TypeError("Don't know how to handle type "+type(target).__name__)


def getAbsName(target: clang.cindex.Cursor) -> str:
	if type(target.semantic_parent) == type(None) or target.semantic_parent.kind == clang.cindex.CursorKind.TRANSLATION_UNIT:
		# Root case: Drop TU name
		return target.displayname
	else:
		# Loop case
		return getAbsName(target.semantic_parent) + "::" + target.displayname


if __name__ == "__main__":
	def _testRig(filePath):
		print(f"Parsing {filePath}:")
		for i in flatten(parseAuto(filePath)):
			print(f"   {getAbsName(i)} : {i.kind}")
		print("")

	_testRig(r"UserPlugin/")
	_testRig(r"PrimitivesPlugin/")
	_testRig(r"engine/memory/public/RawMemoryPool.hpp")
	_testRig(r"engine/")
