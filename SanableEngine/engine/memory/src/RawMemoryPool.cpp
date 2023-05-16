#include "RawMemoryPool.hpp"

#include <cstdlib>
#include <assert.h>
#include <iostream>

#include "TypeInfo.hpp"

using namespace std;

constexpr size_t RawMemoryPool::calcEffectiveObjectSize(size_t baseObjSize)
{
	if (baseObjSize < 4) return 4; //Min: 4
	return getClosestPowerOf2LargerThan(baseObjSize);
}

RawMemoryPool::RawMemoryPool(size_t maxNumObjects, size_t objectSize) :
	dataType(nullptr)
{
	objectSize = calcEffectiveObjectSize(objectSize);

	//allocate the memory
	mMemory = (void*)malloc(mMemoryAllocSize = objectSize * maxNumObjects);

	//set member variables
	mMaxNumObjects = maxNumObjects;
	mNumAllocatedObjects = 0;
	mObjectSize = objectSize;

	//allocate the free list
	mFreeList.clear();
	mFreeList.reserve(mMaxNumObjects);

	//create the free list
	createFreeList();
}

RawMemoryPool::~RawMemoryPool()
{
	//Call release hook on living objects
	if (releaseHook && mNumAllocatedObjects != 0)
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

void RawMemoryPool::updateDataType(TypeInfo const* newType)
{
	if (*dataType != *newType)
	{
		printf("WARNING: Type does not explicitly match (%s => %s). Your instance might crash.", dataType->getAbsName().c_str(), newType->getAbsName().c_str());
	}

	size_t newSize = calcEffectiveObjectSize(newType->getSize());
	if (newSize > mObjectSize)
	{
		//If size grows, addresses are moving forward, so start at high end
		//Index 0 is always in the right spot and can be skipped
		for (int i = mMaxNumObjects-1; i > 0; --i)
		{
			//TODO record address range change
			memcpy_s(((char*)mMemory)+newSize*i, newSize, ((char*)mMemory)+mObjectSize*i, mObjectSize);
		}
	}
	else if (newSize < mObjectSize)
	{
		//If size shrinks, addresses are moving backward, so start at low end
		//Index 0 is always in the right spot and can be skipped
		for (int i = 1; i < mMaxNumObjects; ++i)
		{
			//TODO record address range change
			memcpy_s(((char*)mMemory)+newSize*i, newSize, ((char*)mMemory)+mObjectSize*i, mObjectSize);
		}
	}

	dataType = newType; //TODO make sure is a dynamic allocation rather than static (which will become invalid when reloaded)
}

void RawMemoryPool::refreshVtables(const std::vector<TypeInfo*>& refreshers)
{
	if (!dataType) return; //Can't do anything if we don't have hotswap data. TODO option to acquire hotswap data?

	auto newHotswap = std::find_if(refreshers.cbegin(), refreshers.cend(), [&](TypeInfo* d) { return *d == *dataType; });
	if (newHotswap != refreshers.cend())
	{
		TypeInfo::LayoutRemap layoutRemap = TypeInfo::buildLayoutRemap(dataType, *newHotswap);
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

		dataType = *newHotswap;
	}
	else
	{
		printf("WARNING: Reflection info missing for %s. It will not be remapped. Your instance will likely crash.", dataType->getShortName().c_str());
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
	return mMemory <= ptr && ptr < ((char*)mMemory+mMemoryAllocSize);
}

void* RawMemoryPool::operator[](size_t index) const
{
	void* out = ((uint8_t*)mMemory) + (index * mObjectSize);
	assert(contains(out));
	return out;
}

bool RawMemoryPool::isAlive(void* ptr) const
{
	return std::find(mFreeList.cbegin(), mFreeList.cend(), ptr) == mFreeList.cend();
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