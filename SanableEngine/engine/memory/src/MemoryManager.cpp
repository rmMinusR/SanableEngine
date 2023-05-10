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

void MemoryManager::refreshVtables(std::vector<TypeInfo*> refreshers)
{
	for (auto& p : pools)
	{
		for (TypeInfo* d : refreshers)
		{
			if (*d == *p.poolType) d->vptrJam(p.pool); //No need to TypeInfo::updateLayout here. We can assume Memory Pools will stay the same. Probably.
		}
	}
	for (auto& p : pools) p.pool->refreshVtables(refreshers);
}
