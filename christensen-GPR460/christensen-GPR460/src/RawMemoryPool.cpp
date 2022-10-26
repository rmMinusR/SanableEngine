#include "RawMemoryPool.hpp"

#include <cstdlib>
#include <assert.h>
#include <iostream>

using namespace std;

RawMemoryPool::RawMemoryPool(size_t maxNumObjects, size_t objectSize)
{
	//make objectSize a power of 2 - used for padding
	objectSize = getClosestPowerOf2LargerThan(objectSize);
	if (objectSize < 4)
	{
		objectSize = 4;
	}

	//allocate the memory
	mMemory = (void*)malloc(objectSize * maxNumObjects);

	//set member variables
	mMaxNumObjects = maxNumObjects;
	mNumAllocatedObjects = 0;
	mObjectSize = objectSize;
	mHighestValidAddress = ((uint8_t*)mMemory) + ((maxNumObjects - 1) * objectSize);

	//allocate the free list
	mFreeList.clear();
	mFreeList.reserve(mMaxNumObjects);

	//create the free list
	createFreeList();
}

RawMemoryPool::~RawMemoryPool()
{
	freeRaw(mMemory);
	mFreeList.clear();
}

void RawMemoryPool::reset()
{
	//clear the free list
	mFreeList.clear();
	//create the free list again
	createFreeList();
	//reset count of allocated objects
	mNumAllocatedObjects = 0;
}

void* RawMemoryPool::allocateRaw()
{
	if (mNumAllocatedObjects >= mMaxNumObjects)
	{
		return NULL;
	}

	mNumAllocatedObjects++;
	if (mFreeList.size() > 0)
	{
		void* ptr = mFreeList[0];
		mFreeList.erase(mFreeList.cbegin());
		return ptr;
	}
	else
	{
		assert(false);
		return NULL;
	}
}

void RawMemoryPool::freeRaw(void* ptr)
{
	//make sure that the address passed in is actually one managed by this pool
	if (contains(ptr))
	{
		//add address back to free list
		mFreeList.push_back(ptr);

		mNumAllocatedObjects--;
	}
	else
	{
		cout << "ERROR: object freed from a pool that doesn't manage it\n";
		assert(ptr >= mMemory && ptr <= mHighestValidAddress);
	}
}

bool RawMemoryPool::contains(void* ptr) const
{
	return (ptr >= mMemory && ptr <= mHighestValidAddress);
}

void RawMemoryPool::createFreeList()
{
	for (size_t i = 0; i < mMaxNumObjects; i++)
	{
		mFreeList.push_back(((uint8_t*)mMemory) + (i * mObjectSize));
	}

}
