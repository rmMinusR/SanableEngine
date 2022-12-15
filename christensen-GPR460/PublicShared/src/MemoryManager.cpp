#include "MemoryManager.hpp"

#include "GameObject.hpp"

void MemoryManager::init()
{
	//Nothing to do here
}

void MemoryManager::cleanup()
{
	for (auto& i : pools) delete i.pool;
	pools.clear();
}

void MemoryManager::refreshVtables(std::vector<HotswapTypeData*> refreshers)
{
	for (auto& p : pools)
	{
		for (HotswapTypeData* d : refreshers)
		{
			if (d->name == p.poolType) set_vtable_ptr(p.pool, d->vtable);
		}
	}
}
