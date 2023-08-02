#include "MemoryManager.hpp"

#include "GlobalTypeRegistry.hpp"

void MemoryManager::init()
{
	//Nothing to do here
}

void MemoryManager::cleanup()
{
	for (PoolRecord& i : pools) delete i.pool;
	pools.clear();
}

void MemoryManager::ensureFresh()
{
	std::unordered_set<TypeName> typesToPatch = GlobalTypeRegistry::getDirtyTypes();
	if (typesToPatch.empty()) return; //Halt early if there's nothing to do

	MemoryMapper remapper;

	//Ensure all pools have good vtables
	for (PoolRecord& p : pools)
	{
		auto it = std::find_if(typesToPatch.cbegin(), typesToPatch.cend(), [&](const TypeName& i) { return i == p.poolType.name; });
		if (it != typesToPatch.cend())
		{
			TypeInfo const* newTypeInfo = it->resolve();
			if (newTypeInfo) newTypeInfo->vptrJam(&p.pool);
		}
	}

	//Update the type data for contents of each pool
	for (PoolRecord& p : pools)
	{
		auto it = std::find_if(typesToPatch.cbegin(), typesToPatch.cend(), [&](const TypeName& i) { return i == p.pool->contentsType.name; });
		if (it != typesToPatch.cend())
		{
			TypeInfo const* newTypeInfo = it->resolve();
			if (newTypeInfo) p.pool->refreshObjects(*newTypeInfo, &remapper);
		}
	}

	//Finalize
	updatePointers(remapper);
}

void MemoryManager::updatePointers(const MemoryMapper& remapper)
{
	//TODO implement
}