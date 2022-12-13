#pragma once

#include <cassert>
#include <vector>

#include "MemoryPoolCommon.hpp"

//Void pointers of a given max size (not recommended)
class RawMemoryPool
{
public:
	RawMemoryPool(size_t maxNumObjects, size_t objectSize);
	virtual ~RawMemoryPool();

	//Idiotproofing against myself
	RawMemoryPool(const RawMemoryPool&) = delete;
	RawMemoryPool(RawMemoryPool&&) = default;

	//Allocates raw memory.
	//Should almost always be wrapped by children, since it does no initialization.
	void* allocate();

	//Deallocates raw memory.
	//Children that deal with types should override this to add destructor logic.
	virtual void release(void* obj);

	bool contains(void* ptr) const;
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
};
