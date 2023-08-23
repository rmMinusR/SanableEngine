#pragma once

#include <stdio.h>

#include "GenericTypedMemoryPool.hpp"
#include "MemoryPoolCommon.hpp"


template<typename T, size_t _maxObjectCount = 32>
struct PoolSettings
{
public:
	constexpr static size_t maxObjectCount = _maxObjectCount;
};


//Strongly typed pointers (recommended)
template<typename TObj>
class TypedMemoryPool : public GenericTypedMemoryPool
{
public:
	TypedMemoryPool(size_t maxNumObjects = PoolSettings<TObj>::maxObjectCount) :
		GenericTypedMemoryPool(
			maxNumObjects,
			getClosestPowerOf2LargerThan( std::max(sizeof(TObj), alignof(TObj)) ),
			TypeInfo::createDummy<TObj>()
		)
	{
		if (contentsType.tryRefresh()) releaseHook = contentsType.dtor;
	}

	virtual ~TypedMemoryPool() = default;
public:
	//Allocates memory and creates an object.
	template<typename... TCtorArgs>
	TObj* emplace(TCtorArgs... ctorArgs)
	{
		//Allocate memory
		TObj* pObj = (TObj*) allocate();
		assert(pObj);

		//Construct object
		new (pObj) TObj(ctorArgs...);

		return pObj;
	}

	//Pass through
	inline void release(TObj* obj) { RawMemoryPool::release(obj); }

protected:
	TypedMemoryPool(TypedMemoryPool&&) = default;
	TypedMemoryPool(const TypedMemoryPool&) = delete;
};
