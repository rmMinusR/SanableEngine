#pragma once

#include <stdio.h>

#include "GenericTypedMemoryPool.hpp"
#include "MemoryPoolCommon.hpp"
#include "ObjectPatch.hpp"


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

protected:
	void refreshObjects(const TypeInfo& newTypeData, MemoryMapper* remapper) override
	{
		assert(newTypeData.name == contentsType.name); //Ensure same type
		
		//Resize if we grew
		//Must be done before writing to members so writes don't happen in other objects' memory
		if (newTypeData.size > contentsType.size) resizeObjects(newTypeData.size, remapper);

		//Remap members and write vtable ptrs
		ObjectPatch patch = ObjectPatch::create(contentsType, newTypeData);
		for (size_t i = 0; i < mMaxNumObjects; i++)
		{
			TObj* obj = (TObj*)idToPtr(i);
			if (isAlive(obj)) patch.apply(obj, remapper); //set_vtable_ptr(obj, contentsType.vtable);
		}

		//Resize if we shrunk
		//Must be done after writing to members so we aren't reading other objects' memory
		if (newTypeData.size < contentsType.size) resizeObjects(newTypeData.size, remapper);

		contentsType = newTypeData;

		//Fix bad dtors
		releaseHook = newTypeData.dtor;
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
