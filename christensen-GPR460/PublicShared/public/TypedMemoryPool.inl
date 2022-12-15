#pragma once

#include "RawMemoryPool.hpp"
#include <stdio.h>

template<typename T, size_t _maxObjectCount = 32>
struct PoolSettings
{
public:
	constexpr static size_t maxObjectCount = _maxObjectCount;
};

//Strongly typed pointers (recommended)
template<typename TObj>
class TypedMemoryPool : protected RawMemoryPool
{
public:
	TypedMemoryPool(size_t maxNumObjects = PoolSettings<TObj>::maxObjectCount);
	~TypedMemoryPool();

	//Allocates memory and creates an object.
	template<typename... TCtorArgs>
	TObj* emplace(TCtorArgs... ctorArgs);

	inline void release(TObj* obj) { release((void*)obj); }
protected:
	virtual void release(void* obj) override;

	TypedMemoryPool(TypedMemoryPool&&) = default;
	TypedMemoryPool(const TypedMemoryPool&) = delete;
};



template<typename TObj>
inline TypedMemoryPool<TObj>::TypedMemoryPool(size_t maxNumObjects) :
	RawMemoryPool(maxNumObjects, sizeof(TObj))
{
}

template<typename TObj>
inline TypedMemoryPool<TObj>::~TypedMemoryPool()
{
	//Call dtors on living objects
	for (size_t i = 0; i < mMaxNumObjects; i++)
	{
		TObj* obj = reinterpret_cast<TObj*>(((uint8_t*)mMemory) + (i * mObjectSize));
		bool isAlive = std::find(mFreeList.cbegin(), mFreeList.cend(), obj) == mFreeList.cend();
		if (isAlive)
		{
			printf("WARNING: Unreleased %s@%p", typeid(TObj).name(), obj);
			optional_destructor<TObj>::call(obj);
		}
	}
}

template<typename TObj>
template<typename ...TCtorArgs>
inline TObj* TypedMemoryPool<TObj>::emplace(TCtorArgs ...ctorArgs)
{
	TObj* pObj = reinterpret_cast<TObj*>(allocate());
	assert(pObj);
	//Construct object
	new (pObj) TObj(ctorArgs...);
	return pObj;
}

template<typename TObj>
inline void TypedMemoryPool<TObj>::release(void* obj)
{
	//Manual dtor call since we didn't call delete/memfree
	optional_destructor<TObj>::call((TObj*)obj);

	RawMemoryPool::release(obj);
}