#include "RawMemoryPool.hpp"

#include <cstdlib>
#include <assert.h>
#include <iostream>

using namespace std;

RawMemoryPool::RawMemoryPool() :
	initHook(nullptr),
	releaseHook(nullptr),
	mMemory(nullptr),
	mMaxNumObjects(0),
	mNumAllocatedObjects(0),
	mObjectSize(0),
	mLivingObjects(nullptr)
{
}

RawMemoryPool::RawMemoryPool(size_t maxNumObjects, size_t objectSize) : RawMemoryPool()
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

	//create the free list
	createFreeList();
}

RawMemoryPool::~RawMemoryPool()
{
	//Call release hook on living objects
	if (releaseHook && mNumAllocatedObjects > 0)
	{
		printf("WARNING: A release hook was set, but objects weren't properly released");

		for (size_t i = 0; i < mMaxNumObjects; i++)
		{
			void* obj = idToPtr(i);
			if (isAlive(obj))
			{
				printf(" -> %p", obj);
				releaseHook(obj);
			}
		}
	}

	::free(mMemory);
	mMemory = nullptr;

	::free(mLivingObjects);
	mLivingObjects = nullptr;
}

RawMemoryPool::RawMemoryPool(RawMemoryPool&& rhs) :
	RawMemoryPool(rhs.mMaxNumObjects, rhs.mObjectSize)
{
	std::swap(this->mMemory       , rhs.mMemory       );
	std::swap(this->mLivingObjects, rhs.mLivingObjects);
}

void RawMemoryPool::reset()
{
	//Call release hook on living objects
	if (releaseHook && mNumAllocatedObjects > 0)
	{
		printf("WARNING: A release hook was set, but objects weren't properly released");

		for (size_t i = 0; i < mMaxNumObjects; i++)
		{
			void* obj = idToPtr(i);
			if (isAlive(obj))
			{
				printf(" -> %p", obj);
				releaseHook(obj);
			}
		}
	}
	
	::free(mLivingObjects);
	mLivingObjects = nullptr;

	//create the free list again
	createFreeList();

	//reset count of allocated objects
	mNumAllocatedObjects = 0;
}

void RawMemoryPool::refreshObjects(const std::vector<StableTypeInfo const*>& refreshers)
{

}

void* RawMemoryPool::allocate()
{
	if (mNumAllocatedObjects >= mMaxNumObjects)
	{
		return NULL;
	}

	if (mNumAllocatedObjects < mMaxNumObjects)
	{
		mNumAllocatedObjects++;

		//Scan for first free address
		int id = 0;
		void* ptr;
		for (ptr = idToPtr(id); contains(ptr); ptr = idToPtr(++id))
		{
			if (!isAlive(ptr)) break;
		}

		setFree(id, false);
		if (initHook) initHook(ptr);
		return ptr;
	}
	else
	{
		assert(false);
		return nullptr;
	}
}

void RawMemoryPool::release(void* ptr)
{
	//make sure that the address passed in is actually one managed by this pool
	if (contains(ptr))
	{
		if (releaseHook) releaseHook(ptr);

		//add address back to free list
		setFree(ptrToId(ptr), true);

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
	return idToPtr(0) <= ptr && ptr < idToPtr(mMaxNumObjects);
}

void RawMemoryPool::createFreeList()
{
	assert(!mLivingObjects);

	//Allocate
	float bytesRequired_f = mMaxNumObjects / 8.0f;
	int bytesRequired = std::ceil(bytesRequired_f);
	mLivingObjects = (uint8_t*)malloc(bytesRequired);

	//Mark all unused
	memset(mLivingObjects, 0x00, bytesRequired);
}

void RawMemoryPool::setFree(id_t id, bool isFree)
{
	uint8_t* chunk = mLivingObjects+(id/8);
	uint8_t bitmask = 1 << (id%8);
	if (isFree) *chunk &= ~bitmask;
	else        *chunk |=  bitmask;
}

RawMemoryPool::const_iterator::const_iterator(RawMemoryPool const* pool, id_t index) :
	pool(pool),
	index(index)
{
}

void* RawMemoryPool::const_iterator::operator*() const
{
	return pool->idToPtr(index);
}

RawMemoryPool::const_iterator RawMemoryPool::const_iterator::operator++()
{
	//Advance one
	index++;

	//Skip any IDs that aren't alive
	while (index < pool->mMaxNumObjects && !pool->isAliveById(index)) index++;

	return *this;
}

RawMemoryPool::const_iterator RawMemoryPool::cbegin() const
{
	return const_iterator(this, 0);
}

RawMemoryPool::const_iterator RawMemoryPool::cend() const
{
	return const_iterator(this, mMaxNumObjects);
}