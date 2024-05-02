#pragma once

#include <vector>

#include "dllapi.h"
#include "TypeName.hpp"
#include "TypedMemoryPool.hpp"

class _PoolCallBatcherBase
{
protected:
	TypeName baseTypeName;

	uint64_t cachedStateHash;
	struct CachedPool
	{
		const GenericTypedMemoryPool* pool;
		ParentInfo caster;
	};
	std::vector<CachedPool> cachedPoolList;

	ENGINEMEM_API _PoolCallBatcherBase(const TypeName& baseType);
public:
	ENGINEMEM_API virtual ~_PoolCallBatcherBase();

	ENGINEMEM_API void clear();
	ENGINEMEM_API size_t count() const;
	ENGINEMEM_API void ensureFresh(MemoryManager* src, bool force = false);
};

template<typename TObj>
class PoolCallBatcher : public _PoolCallBatcherBase
{
public:
	PoolCallBatcher() : _PoolCallBatcherBase(TypeName::create<TObj>()) {}
	virtual ~PoolCallBatcher() {}

	template<typename TReturn, typename... TArgs>
	void memberCall(TReturn(TObj::*func)(TArgs...), TArgs... funcArgs) const
	{
		for (const CachedPool& i : cachedPoolList)
		{
			for (auto it = i.pool->cbegin(); it != i.pool->cend(); ++it)
			{
				void* rawObj = *it;
				TObj* obj = (TObj*) i.pool->getContentsType()->layout.upcast(rawObj, i.caster);
				(obj->*func)(funcArgs...);
			}
		}
	}

	template<typename TFunc, typename... TArgs>
	void staticCall(TFunc func, TArgs... funcArgs) const
	{
		for (const CachedPool& i : cachedPoolList)
		{
			for (auto it = i.pool->cbegin(); it != i.pool->cend(); ++it)
			{
				void* rawObj = *it;
				TObj* obj = (TObj*) i.pool->getContentsType()->layout.upcast(rawObj, i.caster);
				func(obj, funcArgs...);
			}
		}
	}
};
