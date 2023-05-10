#include "RawMemoryPool.hpp"

#include <cstdlib>
#include <assert.h>
#include <iostream>

using namespace std;

RawMemoryPool::RawMemoryPool(size_t maxNumObjects, size_t objectSize) :
	hotswap(nullptr)
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
	//Call release hook on living objects
	if (releaseHook && mFreeList.size() < mMaxNumObjects)
	{
		printf("WARNING: A release hook was set, but objects weren't properly released");

		for (size_t i = 0; i < mMaxNumObjects; i++)
		{
			void* obj = ((uint8_t*)mMemory) + (i * mObjectSize);
			bool isAlive = std::find(mFreeList.cbegin(), mFreeList.cend(), obj) == mFreeList.cend();
			if (isAlive)
			{
				printf(" -> %p", obj);
				releaseHook(obj);
			}
		}
	}

	::free(mMemory);
	mFreeList.clear();
}

void RawMemoryPool::reset()
{
	//Call release hook on living objects
	if (releaseHook && mFreeList.size() < mMaxNumObjects)
	{
		printf("WARNING: A release hook was set, but objects weren't properly released");

		for (size_t i = 0; i < mMaxNumObjects; i++)
		{
			void* obj = ((uint8_t*)mMemory) + (i * mObjectSize);
			bool isAlive = std::find(mFreeList.cbegin(), mFreeList.cend(), obj) == mFreeList.cend();
			if (isAlive)
			{
				printf(" -> %p", obj);
				releaseHook(obj);
			}
		}
	}

	//clear the free list
	mFreeList.clear();
	//create the free list again
	createFreeList();
	//reset count of allocated objects
	mNumAllocatedObjects = 0;
}

void RawMemoryPool::refreshVtables(const std::vector<TypeInfo*>& refreshers)
{
	if (!hotswap) return; //Can't do anything if we don't have hotswap data. TODO option to acquire hotswap data?

	auto newHotswap = std::find_if(refreshers.cbegin(), refreshers.cend(), [&](TypeInfo* d) { return *d == *hotswap; });
	if (newHotswap != refreshers.cend())
	{
		TypeInfo::LayoutRemap layoutRemap = TypeInfo::buildLayoutRemap(hotswap, *newHotswap);
		layoutRemap.doSanityCheck(); //Complain if new members are introduced, or old members are deleted

		for (size_t i = 0; i < mMaxNumObjects; i++)
		{
			void* obj = reinterpret_cast<void*>(((uint8_t*)mMemory) + (i * mObjectSize));
			bool isAlive = std::find(mFreeList.cbegin(), mFreeList.cend(), obj) == mFreeList.cend();
			if (isAlive)
			{
				layoutRemap.execute(obj);
				(*newHotswap)->vptrJam(obj);
			}
		}

		hotswap = *newHotswap;
	}
	else
	{
		printf("WARNING: Reflection info missing for %s. It will not be remapped. Your instance will likely crash.", hotswap->getShortName().c_str());
	}
}

void* RawMemoryPool::allocate()
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
		if (initHook) initHook(ptr);
		return ptr;
	}
	else
	{
		assert(false);
		return NULL;
	}
}

void RawMemoryPool::release(void* ptr)
{
	//make sure that the address passed in is actually one managed by this pool
	if (contains(ptr))
	{
		if (releaseHook) releaseHook(ptr);

		//add address back to free list
		mFreeList.push_back(ptr);

		mNumAllocatedObjects--;
	}
	else
	{
		cerr << "ERROR: object freed from a pool that doesn't manage it\n";
		assert(false);
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

RawMemoryPool::const_iterator::const_iterator(RawMemoryPool const* pool, uint8_t* index) :
	pool(pool),
	index(index)
{
}

void* RawMemoryPool::const_iterator::operator*() const
{
	return reinterpret_cast<void*>(index);
}

RawMemoryPool::const_iterator RawMemoryPool::const_iterator::operator++()
{
	do index += pool->getMaxObjectSize();
	while (!pool->isAlive(index) && pool->contains(index));

	return *this;
}

RawMemoryPool::const_iterator RawMemoryPool::cbegin() const
{
	return const_iterator(this, (uint8_t*)mMemory);
}

RawMemoryPool::const_iterator RawMemoryPool::cend() const
{
	return const_iterator(this, ((uint8_t*)mMemory)+(mObjectSize*mMaxNumObjects));
}