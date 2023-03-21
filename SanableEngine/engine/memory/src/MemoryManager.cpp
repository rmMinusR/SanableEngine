#include "MemoryManager.hpp"

void MemoryManager::init()
{
	//Nothing to do here
}

void MemoryManager::cleanup()
{
	for (auto& i : pools) delete i.pool;
	pools.clear();
}

void MemoryManager::refreshVtables(std::vector<StableTypeInfo*> refreshers)
{
	for (auto& p : pools)
	{
		for (StableTypeInfo* d : refreshers)
		{
			if (d->name == p.poolType.name) set_vtable_ptr(p.pool, d->vtable);
		}
	}
	for (auto& p : pools) p.pool->refreshVtables(refreshers);
}
