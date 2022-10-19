#include "MemoryPoolForked.hpp"

#include <cstdlib>
#include <assert.h>
#include <iostream>

using namespace std;

#pragma region From original MemoryPool by Dean Lawson

//got this algorithm from: http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
int isPowerOfTwo(unsigned int x)
{
	return ((x != 0) && !(x & (x - 1)));
}

unsigned int getClosestPowerOf2LargerThan(unsigned int num)
{
	static uint32_t powersOf2[32];
	static bool arrayInitted = false;

	//init an array containing all the powers of 2 
	//(as it is static this should only run the first time this function is called)
	if (!arrayInitted)
	{
		for (uint32_t i = 0; i < 32; i++)
		{
			powersOf2[i] = 1 << i;
		}
	}

	//find the 1st power of 2 which is bigger than or equal to num
	for (uint32_t i = 0; i < 32; i++)
	{
		if ( powersOf2[i] >= num )
			return powersOf2[i];
	}

	//failsafe
	return 0;
	
}

#pragma endregion

MemoryPoolForked::MemoryPoolForked(size_t maxNumObjects, size_t objectSize)
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

MemoryPoolForked::~MemoryPoolForked()
{
	free(mMemory);
	mFreeList.clear();
}

void MemoryPoolForked::reset()
{
	//clear the free list
	mFreeList.clear();
	//create the free list again
	createFreeList();
	//reset count of allocated objects
	mNumAllocatedObjects = 0;
}

void* MemoryPoolForked::allocateObject()
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

void MemoryPoolForked::freeObject(void* ptr)
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

bool MemoryPoolForked::contains(void* ptr) const
{
	return (ptr >= mMemory && ptr <= mHighestValidAddress);
}

void MemoryPoolForked::createFreeList()
{
	for (size_t i = 0; i < mMaxNumObjects; i++)
	{
		mFreeList.push_back(((uint8_t*)mMemory) + (i * mObjectSize));
	}

}
