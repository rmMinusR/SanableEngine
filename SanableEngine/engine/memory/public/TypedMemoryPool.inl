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
//Just a wrapper for GenericTypedMemoryPool. MUST REMAIN NON-VIRTUAL.
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
		//No need to resolve dummy TypeInfo here. Engine will call refreshObjects after all TypeInfos are registered.
	}

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
