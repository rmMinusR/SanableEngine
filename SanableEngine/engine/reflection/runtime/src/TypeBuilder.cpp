#include "TypeBuilder.hpp"

#include "ModuleTypeRegistry.hpp"

TypeBuilder::TypeBuilder()
{
}

void TypeBuilder::addParent_internal(const TypeName& parent, size_t size, const std::function<void* (void*)>& upcastFn, MemberVisibility visibility, ParentInfo::Virtualness virtualness)
{
	pendingParents.emplace_back(parent, type.size, size, upcastFn, visibility, virtualness);
}

void TypeBuilder::addField_internal(const TypeName& declaredType, const std::string& name, size_t size, std::function<ptrdiff_t(const void*)> accessor, MemberVisibility visibility)
{
	pendingFields.emplace_back(declaredType, name, size, accessor, visibility);
}

void TypeBuilder::captureCDO_internal(const std::vector<void*>& instances)
{
	assert(type.byteUsage != nullptr);
	assert(type.implicitValues == nullptr);
	assert(instances.size() >= 2);
	
	//A bitset would be more space-efficient, but that's fixed size and probably slower for our purposes
	memset(type.byteUsage, (uint8_t)TypeInfo::ByteUsage::ImplicitConst, type.size);
	
	//Detect constants
	type.implicitValues = (char*) malloc(type.size);
	memcpy(type.implicitValues, instances[0], type.size); //First one has no point of comparison

	for (int i = 1; i < instances.size(); ++i) //Scan each additional provided instance...
	{
		char* cmp = (char*)instances[i];
		for (size_t byteIndex = 0; byteIndex < type.size; ++byteIndex) //... byte by byte...
		{
			//If we don't match, it's padding
			if (type.implicitValues[byteIndex] != cmp[byteIndex])
			{
				type.byteUsage[byteIndex] = TypeInfo::ByteUsage::Padding;
			}
		}
	}

	//Resolve pending parents
	for (ParentInfoBuilder& p : pendingParents) type.parents.push_back(p.buildFromCDOs(instances));
	pendingParents.clear();

	//Normally we'd scan each field and mark them as used, but we defer until we're registered with GlobalTypeRegistry
	//Otherwise we wouldn't be able to walk parents in the same translation unit for explicit fields
}

void TypeBuilder::registerType(ModuleTypeRegistry* registry)
{
	//Resolve pending parents
	for (ParentInfoBuilder& p : pendingParents) type.parents.push_back(p.buildFromSurrogate());
	pendingParents.clear();

	//Resolve pending fields
	for (const FieldInfoBuilder& p : pendingFields) type.fields.push_back(p.build(type.name, type.implicitValues));
	pendingFields.clear();

	assert(registry->lookupType(type.name) == nullptr);
	registry->types.push_back(type);
}