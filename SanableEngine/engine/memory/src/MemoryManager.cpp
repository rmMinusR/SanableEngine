#include "MemoryManager.hpp"

#include "GlobalTypeRegistry.hpp"

void MemoryManager::init()
{
	//Nothing to do here
}

void MemoryManager::cleanup()
{
	for (GenericTypedMemoryPool* i : pools)
	{
		delete i;
	}
	pools.clear();
}

void MemoryManager::ensureFresh()
{
	std::unordered_set<TypeName> typesToPatch = GlobalTypeRegistry::getDirtyTypes();
	
	MemoryMapper remapper;

	//Update the type data for contents of each pool
	for (GenericTypedMemoryPool* p : pools)
	{
		auto it = std::find_if(typesToPatch.cbegin(), typesToPatch.cend(), [&](const TypeName& i) { return i == p->contentsType.name; });
		if (it != typesToPatch.cend())
		{
			//Existing pools need to be patched
			TypeInfo const* newTypeInfo = it->resolve();
			if (newTypeInfo) p->refreshObjects(*newTypeInfo, &remapper);
		}
		else if (!p->contentsType.isValid())
		{
			//New pools need to be given valid full TypeInfo, rather than dummy
			TypeInfo const* newTypeInfo = p->contentsType.name.resolve();
			if (newTypeInfo) p->refreshObjects(*newTypeInfo, &remapper);
		}
	}

	//Finalize
	updatePointers(remapper);
}

void MemoryManager::updatePointers(const MemoryMapper& remapper)
{
	//TODO implement
}