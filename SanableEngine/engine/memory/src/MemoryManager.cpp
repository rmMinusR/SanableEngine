#include "MemoryManager.hpp"

void MemoryManager::init()
{
	//Nothing to do here
}

void MemoryManager::cleanup()
{
	for (PoolRecord& i : pools) delete i.pool;
	pools.clear();
}

void MemoryManager::refreshObjects(std::vector<StableTypeInfo const*> refreshers)
{
	MemoryMapper mapper; //TODO return and use

	for (PoolRecord& p : pools)
	{
		for (StableTypeInfo const* d : refreshers)
		{
			if (d->name == p.poolType.name) set_vtable_ptr(p.pool, d->vtable);
		}
	}
	
	for (PoolRecord& p : pools) p.pool->refreshObjects(mapper, refreshers);
}
