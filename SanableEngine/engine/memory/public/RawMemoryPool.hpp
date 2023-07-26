#pragma once

#include "dllapi.h"

#include <cassert>
#include <vector>
#include <algorithm>

#include "MemoryPoolCommon.hpp"
#include "StableTypeInfo.inl"

//Void pointers of a given max size (not recommended)
class RawMemoryPool
{
protected:
	typedef uint16_t id_t;

	inline void* idToPtr(id_t id) const
	{
		return ((char*)mMemory) + mObjectSize * id;
	}

	inline id_t ptrToId(void* ptr) const
	{
		assert(contains(ptr));
		ptrdiff_t offset = ((char*)ptr) - ((char*)mMemory);
		assert((offset%mObjectSize) == 0);
		return offset / mObjectSize;
	}

	inline bool isAliveById(id_t id) const
	{
		uint8_t* chunk = mLivingObjects + (id / 8);
		uint8_t bitmask = 1 << (id % 8);
		return *chunk & bitmask;
	}

private:
	ENGINEMEM_API RawMemoryPool();
public:
	ENGINEMEM_API RawMemoryPool(size_t maxNumObjects, size_t objectSize);
	ENGINEMEM_API virtual ~RawMemoryPool();

	//Idiotproofing against myself
	RawMemoryPool(const RawMemoryPool&) = delete;
	RawMemoryPool(RawMemoryPool&&);

	virtual void refreshObjects(const std::vector<StableTypeInfo const*>& refreshers);

	typedef void (*hook_t)(void*);

	//Allocates raw memory.
	//Set hook if type requires special initialization
	ENGINEMEM_API void* allocate();
	hook_t initHook;

	//Deallocates raw memory.
	//Set hook if type requires special cleanup
	ENGINEMEM_API void release(void* obj);
	hook_t releaseHook;

	bool contains(void* ptr) const;
	inline bool isAlive(void* ptr) const
	{
		return isAliveById(ptrToId(ptr)); //Convert to id and defer to main impl
	}
	void reset();//doesn't reallocate memory but does reset free list and num allocated objects

	inline size_t getMaxObjectSize()  const { return mObjectSize; };
	inline size_t getNumFreeObjects() const { return mMaxNumObjects - mNumAllocatedObjects; };
	inline size_t getNumAllocatedObjects() const { return mNumAllocatedObjects; };

protected:
	void* mMemory;
	size_t mMaxNumObjects;
	size_t mNumAllocatedObjects;
	size_t mObjectSize;
	uint8_t* mLivingObjects; //Effectively a dynamically-sized bitset. 1 = alive, 0 = free

	void createFreeList();
	void setFree(id_t id, bool isFree);

public:
	class const_iterator
	{
		RawMemoryPool const* pool;
		id_t index;

		const_iterator(RawMemoryPool const* pool, id_t index);

		friend class RawMemoryPool;

	public:
		ENGINEMEM_API void* operator*() const;

		ENGINEMEM_API const_iterator operator++();

		ENGINEMEM_API inline bool operator!=(const const_iterator& other) const { return pool == other.pool && index != other.index; }
		ENGINEMEM_API inline bool operator==(const const_iterator& other) const { return pool == other.pool && index == other.index; }
	};
	ENGINEMEM_API const_iterator cbegin() const;
	ENGINEMEM_API const_iterator cend() const;
};
