from functools import cached_property
import os
from config import logger
from clang.cindex import *
import zlib

index = Index.create()
additionalCompilerOptions = []

class SourceFile:
	fileTypes = {
		".h"   : "c++-header",
		".hpp" : "c++-header",
		".inl" : "c++-header",
		".c"   : "c++",
		".cpp" : "c++",

		".txt" : None,
		".xml" : None,
		".yml" : None,
		".json" : None
	}

	def __init__(this, filePath: str):
		this.path = filePath
		name, ext = os.path.splitext(this.path)
		this.isGenerated = name.endswith(".generated")
		
		this.type = SourceFile.fileTypes[ext] \
			if ext in SourceFile.fileTypes.keys() \
			else None

		this.hasError = ext not in SourceFile.fileTypes.keys()
		this.tu: TranslationUnit = None
		this.additionalIncludes: list[str] = []
		this.__contentsHash = None

	def __repr__(this):
		return this.path

	def __eq__(this, other):
		return isinstance(other, SourceFile) and this.path == other.path

	def __hash__(this):
		return hash(this.path)

	def __getstate__(this):
		return (this.path, this.isGenerated, this.type, this.hasError, this.__contentsHash)

	def __setstate__(this, d):
		(this.path, this.isGenerated, this.type, this.hasError, this.__contentsHash) = d
		this.tu = None
		this.additionalIncludes = []

	def owns(this, cursor: Cursor) -> bool:
		return this.path == cursor.location.file.name and cursor.is_definition()

	@cached_property
	def name(this):
		return os.path.split(this.path)[-1]

	@cached_property
	def contents(this):
		assert os.path.exists(this.path)
		with open(this.path, "r") as f:
			return f.read()

	@property
	def contentsHash(this):
		if this.__contentsHash == None: 
			this.__contentsHash = zlib.adler32(this.contents.encode("utf-8"))
		return this.__contentsHash

	def parse(this) -> Cursor:
		assert not this.hasError
		assert this.type != None

		if this.tu == None:
			cli_args = ["-std=c++17", "--language="+this.type]
			cli_args.extend(['-I'+i for i in this.additionalIncludes]) # FIXME not space safe!
			cli_args.extend(additionalCompilerOptions)
			try:
				this.tu = index.parse(this.path, args=cli_args, options=TranslationUnit.PARSE_SKIP_FUNCTION_BODIES)
			except Exception:
				logger.critical(f"Error parsing translation unit for target {this.path}", exc_info=True)
				raise

		return this.tu.cursor
	
def discoverAll(targetPaths: list[str]) -> list[SourceFile, None, None]:
	def discover(targetPath: str):
		if os.path.isdir(targetPath):
			# Recurse
			out = []
			for subpath in os.listdir(targetPath):
				out.extend(discover(os.path.join(targetPath, subpath)))
				# Propagate isGenerated if in a .generated directory
				if targetPath.endswith(".generated"):
					for i in out:
						i.isGenerated = True
			return out
		else:
			# Path refers to file
			return [SourceFile(targetPath)]

	out = list()
	for i in targetPaths:
		out.extend(discover(i))
	return out
