#include "MemoryManager.hpp"

#include "GlobalTypeRegistry.hpp"

void MemoryManager::registerPool(GenericTypedMemoryPool* pool)
{
	pools.push_back(pool);

	//Update state hash
	poolStateHash ^= std::hash<TypeName>()(pool->getContentsTypeName());
	poolStateHash = (poolStateHash*1103515245)+12345; //From glibc's rand()
}

GenericTypedMemoryPool* MemoryManager::getSpecificPool(const TypeName& typeName)
{
	//Search for pool matching typename
	auto it = std::find_if(pools.cbegin(), pools.cend(), [&](GenericTypedMemoryPool* p) { return p->getContentsTypeName() == typeName; });
	if (it != pools.cend()) return *it;

	//Nothing found!
	return nullptr;
}

void MemoryManager::foreachPool(const std::function<void(GenericTypedMemoryPool*)>& visitor)
{
	for (GenericTypedMemoryPool* i : pools) visitor(i);
}

void MemoryManager::foreachPool(const std::function<void(const GenericTypedMemoryPool*)>& visitor) const
{
	for (const GenericTypedMemoryPool* i : pools) visitor(i);
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

MemoryManager::MemoryManager()
{
	poolStateHash = 0;
}

MemoryManager::~MemoryManager()
{
	for (GenericTypedMemoryPool* i : pools)
	{
		delete i;
	}
	pools.clear();
}

void MemoryManager::ensureFresh()
{
	std::unordered_set<TypeName> typesToPatch = GlobalTypeRegistry::getDirtyTypes(lastKnownRtti);
	lastKnownRtti = GlobalTypeRegistry::makeSnapshot(); //FIXME slow and potentially unnecessary
	
	MemoryMapper remapper;

	//Update the type data for contents of each pool
	for (GenericTypedMemoryPool* p : pools)
	{
		auto it = std::find_if(typesToPatch.cbegin(), typesToPatch.cend(), [&](const TypeName& i) { return i == p->getContentsTypeName(); });
		if (it != typesToPatch.cend())
		{
			//Existing pools need to be patched
			TypeInfo const* newTypeInfo = it->resolve();
			if (newTypeInfo && newTypeInfo->isLoaded()) p->refreshObjects(*newTypeInfo, &remapper);
		}
		else if (!p->getContentsType() || p->getContentsType()->isDummy())
		{
			//New pools need to be given valid full TypeInfo, rather than dummy
			TypeInfo const* newTypeInfo = p->getContentsTypeName().resolve();
			if (newTypeInfo && newTypeInfo->isLoaded()) p->refreshObjects(*newTypeInfo, &remapper);
		}
	}

	//Finalize
	updatePointers(remapper);
}

void MemoryManager::updatePointers(const MemoryMapper& remapper)
{
	//TODO implement
}

uint64_t MemoryManager::getPoolStateHash() const
{
	return poolStateHash;
}
