#include "TypeBuilder.hpp"

#include "ModuleTypeRegistry.inl"

TypeBuilder::TypeBuilder()
{
}

void TypeBuilder::addParent_internal(const TypeName& parent, size_t offset)
{
	ParentInfo p;
	p.typeName = parent;
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

void TypeBuilder::captureCDO_internal(const std::initializer_list<void*>& instances)
{
	//TODO implement
}

void TypeBuilder::registerType(ModuleTypeRegistry* registry)
{
	assert(registry->lookupType(type.name) == nullptr);
	registry->types.push_back(type);
}