#pragma once

#include "dllapi.h"

#include <cassert>
#include <vector>

#include "MemoryPoolCommon.hpp"
#include "Hotswap.inl"

//Void pointers of a given max size (not recommended)
class RawMemoryPool
{
public:
	ENGINECORE_API RawMemoryPool(size_t maxNumObjects, size_t objectSize);
	ENGINECORE_API virtual ~RawMemoryPool();

	//Idiotproofing against myself
	RawMemoryPool(const RawMemoryPool&) = delete;
	RawMemoryPool(RawMemoryPool&&) = default;

	virtual void refreshVtables(const std::vector<HotswapTypeData*>& refreshers);

	typedef void (*hook_t)(void*);

	//Allocates raw memory.
	//Set hook if type requires special initialization
	ENGINECORE_API void* allocate();
	hook_t initHook = nullptr;

	//Deallocates raw memory.
	//Set hook if type requires special cleanup
	ENGINECORE_API void release(void* obj);
	hook_t releaseHook = nullptr;

	bool contains(void* ptr) const;
	inline bool isAlive(void* ptr) const { return std::find(mFreeList.cbegin(), mFreeList.cend(), ptr) == mFreeList.cend(); }
	void reset();//doesn't reallocate memory but does reset free list and num allocated objects

	inline size_t getMaxObjectSize()  const { return mObjectSize; };
	inline size_t getNumFreeObjects() const { return mMaxNumObjects - mNumAllocatedObjects; };
	inline size_t getNumAllocatedObjects() const { return mNumAllocatedObjects; };

protected:
	void* mMemory;
	void* mHighestValidAddress;
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
		ENGINECORE_API void* operator*() const;

		ENGINECORE_API const_iterator operator++();

		ENGINECORE_API inline bool operator!=(const const_iterator& other) const { return index != other.index; }
		ENGINECORE_API inline bool operator==(const const_iterator& other) const { return index == other.index; }
	};
	ENGINECORE_API const_iterator cbegin() const;
	ENGINECORE_API const_iterator cend() const;
};
