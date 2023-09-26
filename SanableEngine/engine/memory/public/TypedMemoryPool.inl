#pragma once

#include <stdio.h>

#include "GenericTypedMemoryPool.hpp"


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
			TypeInfo::createDummy<TObj>()
		)
	{
		//No need to resolve dummy TypeInfo here. Engine will call refreshObjects after all TypeInfos are registered.
	}

public:
	//Allocates memory and creates an object. Returns null if allocation failed.
	template<typename... TCtorArgs>
	TObj* emplace(TCtorArgs... ctorArgs)
	{
		//Allocate memory
		TObj* pObj = (TObj*) allocate();

		//Construct object (if allocation was valid)
		if (pObj) new (pObj) TObj(ctorArgs...);

		return pObj;
	}

	//Pass through
	inline void release(TObj* obj) { RawMemoryPool::release(obj); }

	//INTERNAL USE ONLY
	inline GenericTypedMemoryPool* asGeneric() { return this; }

protected:
	TypedMemoryPool(TypedMemoryPool&&) = default;
	TypedMemoryPool(const TypedMemoryPool&) = delete;
};
