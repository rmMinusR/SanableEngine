#include "PoolCallBatcher.hpp"

#include "MemoryManager.hpp"

PoolCallBatcher::PoolCallBatcher(const TypeName& baseType) :
	baseTypeName(baseType)
{
}

void PoolCallBatcher::clear()
{
	cachedStateHash = 0;
	cachedPoolList.clear();
}

size_t PoolCallBatcher::count() const
{
	return cachedPoolList.size();
}

void PoolCallBatcher::ensureFresh(MemoryManager* src, bool force)
{
	uint64_t newHash = src->getPoolStateHash();
	if (force || cachedStateHash != newHash)
	{
		cachedStateHash = newHash;

		cachedPoolList.clear();
		src->foreachPool(
			[&](const GenericTypedMemoryPool* pool)
			{
				const TypeInfo* ti = pool->getContentsType();
				if (ti && ti->isDerivedFrom(baseTypeName))
				{
					cachedPoolList.push_back(CachedPool{ pool, ti->getParent(baseTypeName) });
				}
			}
		);
	}
}
