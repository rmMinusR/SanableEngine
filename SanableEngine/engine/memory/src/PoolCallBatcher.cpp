#include "PoolCallBatcher.hpp"

#include "MemoryManager.hpp"

_PoolCallBatcherBase::_PoolCallBatcherBase(const TypeName& baseType, bool skipUnloaded) :
	baseTypeName(baseType),
	skipUnloaded(skipUnloaded)
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
					if (t->name == baseTypeName)
					{
						cachedPoolList.push_back(CachedPool{ pool, ParentInfo::identity(t->name, t->layout.size) });
					}
					else
					{
						std::optional<ParentInfo> p = t->getParent(baseTypeName);
						if (p.has_value()) cachedPoolList.push_back(CachedPool{ pool, p.value() });
					}
				}
			}
		);
	}
}

void _PoolCallBatcherBase::foreachObject(const std::function<void(void*)>& visitor) const
{
	for (const CachedPool& i : cachedPoolList)
	{
		if (!skipUnloaded || i.pool->isLoaded())
		{
			for (auto it = i.pool->cbegin(); it != i.pool->cend(); ++it)
			{
				void* rawObj = *it;
				void* casted = i.pool->getContentsType()->layout.upcast(rawObj, i.caster);
				visitor(casted);
			}
		}
	}
}
