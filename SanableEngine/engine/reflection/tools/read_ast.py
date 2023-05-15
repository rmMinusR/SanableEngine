from clang.cindex import *
import sys, os
from typing import Generator, Iterator

index = Index.create()
fileTypes = {
	".h"   : "c++-header",
	".hpp" : "c++-header",
	".inl" : "c++-header",
	".c"   : "c++",
	".cpp" : "c++"
}

def parseAuto(target: str) -> Generator[Cursor, None, None]:
	if os.path.isdir(target):
		if not target.endswith(".generated"):
			return parseDir(target)
		else:
			print(f"[TRACE] Skipping generated file directory ({target})")
			return None

	else: # Path refers to file
		name, ext = os.path.splitext(target)
		if not name.endswith(".generated"):
			if ext in fileTypes.keys():
				return parseFile(target)
			else:
				print(f"[ WARN] Skipping file of unknown type \"{ext}\" ({target})")
				return None
		else:
			print(f"[TRACE] Skipping generated file ({target})")
			return None

def parseDir(target: str) -> Generator[Cursor, None, None]:
	for subpath in os.listdir(target):
		it = parseAuto(os.path.join(target, subpath))
		if it != None:
			for i in it:
				yield i

def parseFile(target: str) -> Generator[Cursor, None, None]:
	ext = os.path.splitext(target)[1]
	fileType = fileTypes[ext]
	try:
		tu = index.parse(target, args=["-std=c++17", "--language="+fileType])#, options=TranslationUnit.PARSE_SKIP_FUNCTION_BODIES)

		for i in tu.cursor.get_children():
			if isOurs(target, i):
				yield i
		
	except Exception as e:
		print(f"Error parsing translation unit for target {target}")
		raise e

def isOurs(sourceDir: str, i: Cursor):
	return sourceDir == i.location.file.name and i.is_definition()
