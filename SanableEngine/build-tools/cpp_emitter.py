import os

def read_all(path):
	f = open(path, 'r')
	out = '\n'.join(f.readlines())
	f.close()
	return out;

class EmittedFieldInfo:
	def fromAST(line):
		out = EmittedTypeInfo()
		# TODO implement
		return out
	
	def fromProperties(name, typeInfo):
		out = EmittedTypeInfo()
		out.name = name
		out.type = typeInfo
		return out

	#DO NOT CALL DIRECTLY. Use 'from...' functions instead
	def __init__(this):
		pass

	def flatten_value(this) -> str:
		pass #TODO

class EmittedTypeInfo:
	def fromAST(line):
		out = EmittedTypeInfo()
		# TODO implement
		return out
	
	def fromProperties(fullName, displayName=None):
		out = EmittedTypeInfo()
		out.fullName = fullName
		out.displayName = displayName if (displayName != None) else fullName
		return out

	#DO NOT CALL DIRECTLY. Use 'from...' functions instead
	def __init__(this):
		this.fields = []

	def add_field(this, field: EmittedFieldInfo):
		this.fields.append(field)

	template = """
	StableTypeInfo("{{fullName}}", {
		{{fields}}
	})"""

	def flatten_value(this) -> str:
		return template.format(fullName=fullName, fields='\n\t\t'.join([i.flatten_value() for i in this.fields]))

class EmittedTypeRegistry:
	def __init__(this):
		this.types = []

	template = """
#include "StableTypeInfo.hpp"

TypeRegistry{{exported_name}} = TypeRegistry({
	{{types}}
});
"""
	def flatten(this, exported_name) -> str:
		types_flattened = ''.join([i.flatten_value() for i in this.types])
		return template.format(exported_name=exported_name, types=types_flattened)

	def add_type(this, type: EmittedTypeInfo):
		this.types.append(type)