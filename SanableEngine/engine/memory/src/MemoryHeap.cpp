#include "MemoryHeap.hpp"

#include "GlobalTypeRegistry.hpp"
#include "MemoryRoot.hpp"

void MemoryHeap::registerPool(GenericTypedMemoryPool* pool)
{
	pools.push_back(pool);

	//Update state hash
	poolStateHash ^= std::hash<TypeName>()(pool->getContentsTypeName());
	poolStateHash = (poolStateHash*1103515245)+12345; //From glibc's rand()
}

GenericTypedMemoryPool* MemoryHeap::getSpecificPool(const TypeName& typeName)
{
	//Search for pool matching typename
	auto it = std::find_if(pools.cbegin(), pools.cend(), [&](GenericTypedMemoryPool* p) { return p->getContentsTypeName() == typeName; });
	if (it != pools.cend()) return *it;

	//Nothing found!
	return nullptr;
}

void MemoryHeap::foreachPool(const std::function<void(GenericTypedMemoryPool*)>& visitor)
{
	for (GenericTypedMemoryPool* i : pools) visitor(i);
}

void MemoryHeap::foreachPool(const std::function<void(const GenericTypedMemoryPool*)>& visitor) const
{
	for (const GenericTypedMemoryPool* i : pools) visitor(i);
}

void MemoryHeap::destroyPool(const TypeName& type)
{
	auto it = std::find_if(pools.cbegin(), pools.cend(), [&](GenericTypedMemoryPool* p) { return p->getContentsTypeName() == type; });
	if (it != pools.cend())
	{
		delete *it;
		pools.erase(it);
	}
}

MemoryHeap::MemoryHeap()
{
	poolStateHash = 0;

	//Mark alive
	MemoryRoot::get()->registerHeap(this);
}

MemoryHeap::~MemoryHeap()
{
	//Mark dead/dying
	MemoryRoot::get()->removeHeap(this);

	for (GenericTypedMemoryPool* i : pools)
	{
		delete i;
	}
	pools.clear();
}

void MemoryHeap::ensureFresh(bool selfOnly)
{
	MemoryMapper remapper;

	//Fix new pools that haven't received their complete TypeInfo yet
	for (GenericTypedMemoryPool* p : pools)
	{
		if (!p->getContentsType() || p->getContentsType()->isDummy())
		{
			//New pools need to be given valid full TypeInfo, rather than dummy
			TypeInfo const* newTypeInfo = p->getContentsTypeName().resolve();
			if (newTypeInfo && newTypeInfo->isLoaded()) p->refreshObjects(*newTypeInfo, &remapper);
		}
	}

	//Handle changes to existing pools
	std::unordered_set<TypeName> typesToPatch = GlobalTypeRegistry::getDirtyTypes(lastKnownRtti);
	lastKnownRtti = GlobalTypeRegistry::makeSnapshot(); //FIXME slow and potentially unnecessary
	
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
	}

	//Finalize
	std::set<void*> visitRecord;
	if (selfOnly) updatePointers(remapper, visitRecord);
	else MemoryRoot::get()->updatePointers(remapper, visitRecord);
}

void MemoryHeap::updatePointers(const MemoryMapper& remapper, std::set<void*>& visitRecord)
{
	for (GenericTypedMemoryPool* p : pools)
	{
		const TypeInfo* ty = p->getContentsType();
		for (auto it = p->cbegin(); it != p->cend(); ++it)
		{
			remapper.transformObjectAddresses(*it, ty, true, &visitRecord);
		}
	}
}

uint64_t MemoryHeap::getPoolStateHash() const
{
	return poolStateHash;
}
