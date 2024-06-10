#pragma once

#include <vector>
#include <functional>

#include "dllapi.h"
#include "TypeName.hpp"
#include "TypedMemoryPool.hpp"

class _PoolCallBatcherBase
{
protected:
	TypeName baseTypeName;
	bool skipUnloaded;

	uint64_t cachedStateHash;
	struct CachedPool
	{
		const GenericTypedMemoryPool* pool;
		ParentInfo caster;
	};
	std::vector<CachedPool> cachedPoolList;

	ENGINEMEM_API _PoolCallBatcherBase(const TypeName& baseType, bool skipUnloaded = true);
public:
	ENGINEMEM_API virtual ~_PoolCallBatcherBase();

	ENGINEMEM_API void clear();
	ENGINEMEM_API size_t count() const;
	ENGINEMEM_API void ensureFresh(MemoryManager* src, bool force = false);

	ENGINEMEM_API void foreachObject(const std::function<void(void*)>& visitor) const; //Given pointer will already have been cast to correct type
};

template<typename TObj>
class PoolCallBatcher : public _PoolCallBatcherBase
{
public:
	PoolCallBatcher(bool skipUnloaded = true) : _PoolCallBatcherBase(TypeName::create<TObj>(), skipUnloaded) {}
	virtual ~PoolCallBatcher() {}

	template<typename TReturn, typename... TArgs>
	void memberCall(TReturn(TObj::*func)(TArgs...), TArgs... funcArgs) const
	{
		foreachObject([&](void* obj) { (static_cast<TObj*>(obj)->*func)(funcArgs...); });
	}

	template<typename TReturn, typename... TArgs>
	void memberCall(TReturn(TObj::* func)(TArgs...) const, TArgs... funcArgs) const
	{
		foreachObject([&](void* obj) { (static_cast<TObj*>(obj)->*func)(funcArgs...); });
	}

	template<typename TFunc, typename... TArgs>
	void staticCall(TFunc func, TArgs... funcArgs) const
	{
		foreachObject([&](void* obj) { func(static_cast<TObj*>(obj), funcArgs...); });
	}
};
