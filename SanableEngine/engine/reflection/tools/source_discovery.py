from functools import cached_property
import os
from config import logger
from clang.cindex import *
import zlib
import glob

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
		this.path = filePath.replace(os.altsep, os.sep)
		name, ext = os.path.splitext(this.path)
		this.isGenerated = name.endswith(".generated")
		
		this.type = SourceFile.fileTypes[ext] \
			if ext in SourceFile.fileTypes.keys() \
			else None

		this.hasError = ext not in SourceFile.fileTypes.keys()
		this.tu: TranslationUnit = None
		this.additionalIncludes: list[str] = []
		
		assert os.path.exists(this.path)
		with open(this.path, "r") as f:
			this.contents = f.read()
		this.contentsHash = zlib.adler32(this.contents.encode("utf-8"))

	def __repr__(this):
		return this.path

	def __eq__(this, other):
		return isinstance(other, SourceFile) and this.path == other.path

	def __hash__(this):
		return hash(this.path)

	def __getstate__(this):
		return (this.path, this.isGenerated, this.type, this.hasError, this.contentsHash)

	def __setstate__(this, d):
		(this.path, this.isGenerated, this.type, this.hasError, this.contentsHash) = d
		this.tu = None
		this.additionalIncludes = []

	def owns(this, cursor: Cursor) -> bool:
		return this.path == cursor.location.file.name and cursor.is_definition()

	@cached_property
	def name(this):
		return os.path.split(this.path)[-1]

	def parse(this) -> Cursor:
		assert not this.hasError
		assert this.type != None

		if this.tu == None:
			cli_args = ["-std=c++17", "--language="+this.type, "-D__STIX_REFELCTION_GENERATING"]
			cli_args.extend(['-I'+i for i in this.additionalIncludes]) # FIXME not space safe!
			cli_args.extend(additionalCompilerOptions)
			try:
				this.tu = index.parse(this.path, args=cli_args, options=TranslationUnit.PARSE_SKIP_FUNCTION_BODIES)
			except Exception:
				logger.critical(f"Error parsing translation unit for target {this.path}", exc_info=True)
				raise

		return this.tu.cursor
	
def discoverAll(targetPaths: list[str]) -> list[SourceFile, None, None]:
	targetFiles = []
	for p in targetPaths: targetFiles += glob.glob(p+"/**", recursive=True)
	return [SourceFile(i) for i in sorted(set(targetFiles)) if not os.path.isdir(i)]

class ProjectDiff:
	def __init__(this, oldFiles: list[SourceFile], newFiles: list[SourceFile]):
		isUpToDate = lambda file: next(filter(lambda i: i.path == file.path, oldFiles), None).contentsHash == next(filter(lambda i: i.path == file.path, newFiles), None).contentsHash
		existedPreviously = lambda file: any((i.path == file.path for i in oldFiles))
		existsCurrently = lambda file: any((i.path == file.path for i in newFiles))
		
		this.upToDate = [i for i in newFiles if existedPreviously(i) and isUpToDate(i)]
		this.outdated = [i for i in newFiles if existedPreviously(i) and not isUpToDate(i)]
		this.new     = [i for i in newFiles if not existedPreviously(i)]
		this.removed = [i for i in oldFiles if not existsCurrently(i)]
		
		pass

	def __str__(this):
		return f"{len(this.upToDate)} up-to-date | {len(this.outdated)} outdated | {len(this.new)} new | {len(this.removed)} deleted"
	