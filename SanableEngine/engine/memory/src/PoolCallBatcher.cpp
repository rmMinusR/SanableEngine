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
				const TypeInfo* t = pool->getContentsType();
				if (t)
				{
					std::optional<ParentInfo> p = t->getParent(baseTypeName);
					if (p.has_value()) cachedPoolList.push_back(CachedPool{ pool, p.value() });
				}
			}
		);
	}
}
