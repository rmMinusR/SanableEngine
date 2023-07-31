#include "GlobalTypeRegistry.hpp"

#include <unordered_set>

std::unordered_map<GlobalTypeRegistry::module_key_t, ModuleTypeRegistry> GlobalTypeRegistry::modules;
std::vector<ObjectPatch> GlobalTypeRegistry::pendingPatches;

StableTypeInfo const* GlobalTypeRegistry::lookupType(const TypeName& name)
{
	for (const auto& i : modules)
	{
		StableTypeInfo const* out = i.second.lookupType(name);
		if (out) return out;
	}
	return nullptr;
}

void GlobalTypeRegistry::addModule(module_key_t key, ModuleTypeRegistry newTypes)
{
	assert(modules.find(key) == modules.cend());
	modules.emplace(key, newTypes);
}

void GlobalTypeRegistry::updateModule(module_key_t key, ModuleTypeRegistry newTypes)
{
	auto it = modules.find(key);
	assert(it != modules.cend());
	ModuleTypeRegistry& oldTypes = it->second;

	//Collect names from both old and new version
	std::unordered_set<TypeName> allNames;
	for (const StableTypeInfo& i : oldTypes.getTypes()) allNames.emplace(i.name);
	for (const StableTypeInfo& i : newTypes.getTypes()) allNames.emplace(i.name);

	//Produce patch for each name
	for (const TypeName& i : allNames)
	{
		ObjectPatch objPatch;
		objPatch.oldData = *oldTypes.lookupType(i);
		objPatch.newData = *newTypes.lookupType(i);
		assert(objPatch.isValid());
		pendingPatches.push_back(objPatch);
	}

	//Update backing data
	it->second = newTypes;
}

std::vector<ObjectPatch> GlobalTypeRegistry::exportPatch()
{
	std::vector<ObjectPatch> out;
	std::swap(out, pendingPatches);
	return out;
}
