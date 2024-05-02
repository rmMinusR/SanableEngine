#include "PoolCallBatcher.hpp"

#include "MemoryManager.hpp"

_PoolCallBatcherBase::_PoolCallBatcherBase(const TypeName& baseType) :
	baseTypeName(baseType)
{
}

_PoolCallBatcherBase::~_PoolCallBatcherBase()
{
}

void _PoolCallBatcherBase::clear()
{
	cachedStateHash = 0;
	cachedPoolList.clear();
}

size_t _PoolCallBatcherBase::count() const
{
	return cachedPoolList.size();
}

void _PoolCallBatcherBase::ensureFresh(MemoryManager* src, bool force)
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
