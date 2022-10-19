#pragma once
#include <vector>

//A statically-sized memory pool
//Heavily modified, signatures originally based on work by Dean Lawson

class MemoryPoolForked
{
public:
	MemoryPoolForked(size_t maxNumObjects, size_t objectSize);
	~MemoryPoolForked();

	MemoryPoolForked(const MemoryPoolForked&) = delete;
	MemoryPoolForked(MemoryPoolForked&&) = default;

	void reset();//doesn't reallocate memory but does reset free list and num allocated objects

	void* allocateObject();
	void freeObject(void* ptr);
	
	inline size_t getMaxObjectSize()  const { return mObjectSize; };
	inline size_t getNumFreeObjects() const { return mMaxNumObjects - mNumAllocatedObjects; };
	inline size_t getNumAllocatedObjects() const { return mNumAllocatedObjects; };

	bool contains(void* ptr) const;

private:
	void* mMemory;
	void* mHighestValidAddress;
	size_t mMaxNumObjects;
	size_t mNumAllocatedObjects;
	size_t mObjectSize;
	std::vector<void*> mFreeList;

	void createFreeList();
};