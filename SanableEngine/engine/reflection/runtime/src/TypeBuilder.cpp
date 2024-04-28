#include "TypeBuilder.hpp"

#include "ModuleTypeRegistry.hpp"

TypeBuilder::TypeBuilder()
{
}

void TypeBuilder::addParent_internal(const TypeName& parent, size_t size, const std::function<void* (void*)>& upcastFn, MemberVisibility visibility, ParentInfo::Virtualness virtualness)
{
	pendingParents.emplace_back(type.name, parent, type.size, size, upcastFn, visibility, virtualness);
}

void TypeBuilder::addField_internal(const TypeName& declaredType, const std::string& name, size_t size, std::function<ptrdiff_t(const void*)> accessor, MemberVisibility visibility)
{
	pendingFields.emplace_back(declaredType, name, size, accessor, visibility);
}

void TypeBuilder::captureClassImage_v1_internal(std::function<void(void*)> ctor, std::function<void(void*)> dtor)
{
	assert(type.byteUsage != nullptr);
	assert(type.implicitValues == nullptr);
	
	//Prepare memory
	char* cdo1 = (char*)malloc(3 * type.size);
	char* cdo2 = cdo1 + type.size;
	char* cdo3 = cdo2 + type.size;
	memset(cdo1, 0x00, type.size);
	memset(cdo2, 0x88, type.size);
	memset(cdo3, 0xFF, type.size);

	//Create CDOs
	ctor(cdo1);
	ctor(cdo2);
	ctor(cdo3);

	////// BEGIN MAIN MAGIC //////

	//A bitset would be more space-efficient, but that's fixed size and probably slower for our purposes
	memset(type.byteUsage, (uint8_t)TypeInfo::ByteUsage::ImplicitConst, type.size);
	
	//Detect constants
	type.implicitValues = (char*) malloc(type.size);
	memcpy(type.implicitValues, cdo1, type.size); //First one has no point of comparison

	auto cmpAgainst = [&](char* cmp)
	{
		for (size_t byteIndex = 0; byteIndex < type.size; ++byteIndex) //... byte by byte...
		{
			//If we don't match, it's padding
			if (type.implicitValues[byteIndex] != cmp[byteIndex])
			{
				type.byteUsage[byteIndex] = TypeInfo::ByteUsage::Padding;
			}
		}
	};
	cmpAgainst(cdo2);
	cmpAgainst(cdo3);

	//Normally we'd scan each field and mark them as used, but we defer until we're registered with GlobalTypeRegistry
	//Otherwise we wouldn't be able to walk parents in the same translation unit for explicit fields

	////// END MAIN MAGIC //////
	
	dtor(cdo1);
	dtor(cdo2);
	dtor(cdo3);
	free(cdo1);
}

void TypeBuilder::captureClassImage_v2_internal(const DetectedConstants& image)
{
	assert(type.byteUsage != nullptr);
	assert(type.implicitValues == nullptr);
	type.implicitValues = (char*)malloc(type.size);

	//Assume padding as default
	memset(type.byteUsage, (uint8_t)TypeInfo::ByteUsage::Padding, type.size);

	for (int i = 0; i < type.size; ++i)
	{
		if (image.usage[i])
		{
			type.byteUsage[i] = TypeInfo::ByteUsage::ImplicitConst;
			type.implicitValues[i] = image.bytes[i];
		}
	}
}

void TypeBuilder::registerType(ModuleTypeRegistry* registry)
{
	//Resolve pending parents
	for (ParentInfoBuilder& p : pendingParents) type.parents.push_back(p.buildFromClassImage(type.implicitValues));
	pendingParents.clear();

	//Resolve pending fields
	for (const FieldInfoBuilder& p : pendingFields) type.fields.push_back(p.build(type.name, type.implicitValues));
	pendingFields.clear();

	//DO NOT update byte usage: this is deferred to TypeInfo::doLateBinding

	assert(registry->lookupType(type.name) == nullptr);
	registry->types.push_back(type);
}