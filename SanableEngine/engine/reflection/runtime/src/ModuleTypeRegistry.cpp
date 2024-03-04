#include "ModuleTypeRegistry.hpp"

void ModuleTypeRegistry::doLateBinding()
{
	for (TypeInfo& i : types) i.doLateBinding();
}

TypeInfo const* ModuleTypeRegistry::lookupType(const TypeName& name) const
{
	for (const TypeInfo& i : types) if (i.name == name) return &i;
	return nullptr;
}

const std::vector<TypeInfo>& ModuleTypeRegistry::getTypes() const
{
	return types;
}

TypeInfo const* ModuleTypeRegistry::snipeType(void* obj, size_t size, TypeInfo const* hint) const
{
	for (const TypeInfo& i : types)
	{
		if (i.size <= size && i.matchesExact(obj)) return &i;
	}
	return nullptr;
}
