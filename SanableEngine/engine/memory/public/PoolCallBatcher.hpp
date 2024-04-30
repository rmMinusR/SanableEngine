#pragma once

#include <algorithm>
#include <vector>
#include <cassert>

#include "dllapi.h"
#include "TypeName.hpp"
#include "TypedMemoryPool.hpp"

class PoolCallBatcher
{
private:
	TypeName baseTypeName;

	uint64_t cachedStateHash;
	struct CachedPool
	{
		const GenericTypedMemoryPool* pool;
		ParentInfo caster;
	};
	std::vector<CachedPool> cachedPoolList;

public:
	ENGINEMEM_API PoolCallBatcher(const TypeName& baseType);

	ENGINEMEM_API void clear();
	ENGINEMEM_API size_t count() const;

	template<typename TObj, typename... TArgs>
	void memberCall(void (TObj::*func)(TArgs...), TArgs... funcArgs) const
	{
		assert(TypeName::create<TObj>() == baseTypeName);
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

	template<typename TObj, typename TFunc, typename... TArgs>
	void staticCall(TFunc func, TArgs... funcArgs) const
	{
		assert(TypeName::create<TObj>() == baseTypeName);
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

	ENGINEMEM_API void ensureFresh(MemoryManager* src, bool force = false);
};
