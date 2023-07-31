#pragma once

#include "RawMemoryPool.hpp"

#include <stdio.h>

template<typename T, size_t _maxObjectCount = 32>
struct PoolSettings
{
public:
	constexpr static size_t maxObjectCount = _maxObjectCount;
};


//Wrapper for TypedMemoryPool so we can still safely access common data
class GenericTypedMemoryPool : protected RawMemoryPool
{
protected:
	StableTypeInfo hotswap;

	friend class MemoryManager;

	virtual void refreshObjects(const StableTypeInfo& newTypeData, MemoryMapper* remapper) = 0;

public:
	ENGINEMEM_API inline GenericTypedMemoryPool(size_t maxNumObjects, size_t objectSize, StableTypeInfo hotswap) : RawMemoryPool(maxNumObjects, objectSize), hotswap(hotswap) {}
	ENGINEMEM_API virtual ~GenericTypedMemoryPool() = default;
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
			StableTypeInfo::blank<TObj>()
		)
	{
		releaseHook = (RawMemoryPool::hook_t) optional_destructor<TObj>::call;
	}

	virtual ~TypedMemoryPool() = default;

protected:
	void refreshObjects(const StableTypeInfo& newTypeData, MemoryMapper* remapper) override
	{
		assert(newTypeData.name == hotswap.name); //Ensure same type
		
		//Resize if we grew
		//Must be done before writing to members so writes don't happen in other objects' memory
		if (newTypeData.size > hotswap.size) resizeObjects(newTypeData.size, remapper);

		//Remap members and write vtable ptrs
		ObjectPatch patch = ObjectPatch::create(hotswap, newTypeData);
		for (size_t i = 0; i < mMaxNumObjects; i++)
		{
			TObj* obj = (TObj*)idToPtr(i);
			if (isAlive(obj)) patch.apply(obj, remapper); //set_vtable_ptr(obj, hotswap.vtable);
		}

		//Resize if we shrunk
		//Must be done after writing to members so we aren't reading other objects' memory
		if (newTypeData.size < hotswap.size) resizeObjects(newTypeData.size, remapper);

		hotswap = newTypeData;

		//Fix bad dtors - TODO pull from StableTypeInfo
		releaseHook = (RawMemoryPool::hook_t)optional_destructor<TObj>::call;
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

		//Extract vtable (if not initted)
		if (!hotswap.vtable) hotswap.set_vtable(pObj);

		return pObj;
	}

	//Pass through
	inline void release(TObj* obj) { RawMemoryPool::release(obj); }

protected:
	TypedMemoryPool(TypedMemoryPool&&) = default;
	TypedMemoryPool(const TypedMemoryPool&) = delete;
};
