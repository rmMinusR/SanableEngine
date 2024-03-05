#include "MemoryManager.hpp"

#include "GlobalTypeRegistry.hpp"

GenericTypedMemoryPool* MemoryManager::getSpecificPool(const TypeName& typeName)
{
	//Search for pool matching typename
	auto it = std::find_if(pools.cbegin(), pools.cend(), [&](GenericTypedMemoryPool* p) { return p->getContentsTypeName() == typeName; });
	if (it != pools.cend()) return *it;

	//Nothing found!
	return nullptr;
}

void MemoryManager::destroyPool(const TypeName& type)
{
	auto it = std::find_if(pools.cbegin(), pools.cend(), [&](GenericTypedMemoryPool* p) { return p->getContentsTypeName() == type; });
	if (it != pools.cend())
	{
		delete *it;
		pools.erase(it);
	}
}

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
		auto it = std::find_if(typesToPatch.cbegin(), typesToPatch.cend(), [&](const TypeName& i) { return i == p->getContentsTypeName(); });
		if (it != typesToPatch.cend())
		{
			//Existing pools need to be patched
			TypeInfo const* newTypeInfo = it->resolve();
			if (newTypeInfo) p->refreshObjects(*newTypeInfo, &remapper);
		}
		else if (!p->getContentsType())
		{
			//New pools need to be given valid full TypeInfo, rather than dummy
			TypeInfo const* newTypeInfo = p->getContentsTypeName().resolve();
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