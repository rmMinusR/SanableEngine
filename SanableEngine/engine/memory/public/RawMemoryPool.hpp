#pragma once

#include "dllapi.h"

#include <cassert>
#include <vector>
#include <algorithm>

#include "MemoryPoolCommon.hpp"
#include "TypeInfo.hpp"

//Void pointers of a given max size (not recommended)
class RawMemoryPool
{
protected:
	TypeInfo* dataType; //Dynamic, owned by self. Version owned by DLL would become invalid when reloaded.

	ENGINEMEM_API static constexpr size_t calcEffectiveObjectSize(size_t baseObjSize);

public:
	ENGINEMEM_API RawMemoryPool(size_t maxNumObjects, size_t objectSize);
	ENGINEMEM_API ~RawMemoryPool();

	//Idiotproofing against myself
	RawMemoryPool(const RawMemoryPool&) = delete;
	RawMemoryPool(RawMemoryPool&&) = default;

	ENGINEMEM_API void updateDataType(TypeInfo const* newType);
	ENGINEMEM_API void refreshVtables(const std::vector<TypeInfo*>& refreshers);

	typedef void (*hook_t)(void*);

	//Allocates raw memory.
	//Set hook if type requires special initialization
	ENGINEMEM_API void* allocate();
	hook_t initHook = nullptr;

	//Deallocates raw memory.
	//Set hook if type requires special cleanup
	ENGINEMEM_API void release(void* obj);
	hook_t releaseHook = nullptr;

	ENGINEMEM_API bool contains(void* ptr) const;
	ENGINEMEM_API void* operator[](size_t index) const;
	ENGINEMEM_API bool isAlive(void* ptr) const;
	ENGINEMEM_API void reset();//doesn't reallocate memory but does reset free list and num allocated objects

	inline size_t getMaxObjectSize() const { return mObjectSize; };
	inline size_t getNumFreeObjects() const { return mMaxNumObjects - mNumAllocatedObjects; };
	inline size_t getNumAllocatedObjects() const { return mNumAllocatedObjects; };

protected:
	void* mMemory;
	size_t mMemoryAllocSize;
	size_t mMaxNumObjects;
	size_t mNumAllocatedObjects;
	size_t mObjectSize;
	std::vector<void*> mFreeList;

	void createFreeList();

public:
	class const_iterator
	{
		RawMemoryPool const* pool;
		uint8_t* index;

		const_iterator(RawMemoryPool const* pool, uint8_t* index);

		friend class RawMemoryPool;

	public:
		ENGINEMEM_API void* operator*() const;

		ENGINEMEM_API const_iterator operator++();

		ENGINEMEM_API inline bool operator!=(const const_iterator& other) const { return index != other.index; }
		ENGINEMEM_API inline bool operator==(const const_iterator& other) const { return index == other.index; }
	};
	ENGINEMEM_API const_iterator cbegin() const;
	ENGINEMEM_API const_iterator cend() const;
};
