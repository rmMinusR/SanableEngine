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
	std::vector<const GenericTypedMemoryPool*> cachedPoolList;

public:
	ENGINEMEM_API PoolCallBatcher(const TypeName& baseType);

	ENGINEMEM_API void clear();

	template<typename TObj, typename... TArgs>
	void memberCall(void (TObj::*func)(TArgs...), TArgs... funcArgs) const
	{
		assert(TypeName::create<TObj>() == baseTypeName);
		for (const GenericTypedMemoryPool* i : cachedPoolList)
		{
			for (auto it = i->cbegin(); it != i->cend(); ++it)
			{
				(static_cast<TObj*>(*it)->*func)(funcArgs...);
			}
		}
	}

	template<typename TObj, typename TFunc, typename... TArgs>
	void staticCall(TFunc func, TArgs... funcArgs) const
	{
		assert(TypeName::create<TObj>() == baseTypeName);
		for (const GenericTypedMemoryPool* i : cachedPoolList)
		{
			for (auto it = i->cbegin(); it != i->cend(); ++it)
			{
				func(static_cast<TObj*>(*it), funcArgs...);
			}
		}
	}

	ENGINEMEM_API void ensureFresh(MemoryManager* src, bool force = false);
};
