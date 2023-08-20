#include "TypeBuilder.hpp"

#include "ModuleTypeRegistry.hpp"

TypeBuilder::TypeBuilder()
{
}

void TypeBuilder::addParent_internal(const TypeName& parent, size_t size, size_t offset)
{
	ParentInfo p;
	p.typeName = parent;
	p.size = size;
	p.offset = offset;
	type.parents.push_back(p);
}

void TypeBuilder::addField_internal(const TypeName& declaredType, const std::string& name, size_t size, size_t offset)
{
	FieldInfo f;
	f.type = declaredType;
	f.name = name;
	f.size = size;
	f.offset = offset;
	type.fields.push_back(f);
}

void TypeBuilder::captureCDO_internal(const std::vector<void*>& instances)
{
	assert(type.implicitsMask == nullptr);
	assert(type.implicitValues == nullptr);
	assert(instances.size() >= 2);

	//A bitset would be more space-efficient, but that's fixed size and probably slower for our purposes
	type.implicitsMask = (char*) malloc(type.size); //If false, something is occupying this byte
	memset(type.implicitsMask, 0xFF, type.size);

	//Detect constants
	type.implicitValues = (char*) malloc(type.size);
	memcpy(type.implicitValues, instances[0], type.size); //First one has no point of comparison

	for (int i = 1; i < instances.size(); ++i) //Scan each additional provided instance...
	{
		char* cmp = (char*)instances[i];
		for (size_t byteIndex = 0; byteIndex < type.size; ++byteIndex) //... byte by byte...
		{
			//If we don't match, it's padding
			type.implicitsMask[byteIndex] &= (
				type.implicitValues[byteIndex] == cmp[byteIndex]
			);
		}
	}

	//Normally we'd scan each field and mark them as used, but we defer until we're registered with GlobalTypeRegistry
	//Otherwise we wouldn't be able to walk parents in the same translation unit
}

void TypeBuilder::registerType(ModuleTypeRegistry* registry)
{
	assert(registry->lookupType(type.name) == nullptr);
	registry->types.push_back(type);
}