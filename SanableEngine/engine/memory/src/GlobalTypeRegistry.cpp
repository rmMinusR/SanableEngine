#include "GlobalTypeRegistry.hpp"

std::unordered_map<GlobalTypeRegistry::module_key_t, ModuleTypeRegistry> GlobalTypeRegistry::modules;
MemoryMapper GlobalTypeRegistry::pendingPatch;

void GlobalTypeRegistry::addModule(module_key_t key, ModuleTypeRegistry types)
{
	assert(modules.find(key) == modules.cend());
	modules.emplace(key, types);
}

void GlobalTypeRegistry::updateModule(module_key_t key, ModuleTypeRegistry types)
{
	auto it = modules.find(key);
	assert(it != modules.cend());

	//TODO update patch!

	it->second = types;
}

MemoryMapper GlobalTypeRegistry::exportPatch()
{
	MemoryMapper out;
	std::swap(out, pendingPatch);
	return out;
}
