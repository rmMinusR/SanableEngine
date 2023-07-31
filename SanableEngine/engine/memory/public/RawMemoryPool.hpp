#pragma once

#include "dllapi.h"

#include <cassert>
#include <vector>
#include <algorithm>
#include <string>

#include "MemoryMapper.hpp"

//Void pointers of a given max size (not recommended)
class RawMemoryPool
{
protected:
	typedef uint16_t id_t;

	ENGINEMEM_API void* idToPtr(id_t id) const;
	ENGINEMEM_API id_t ptrToId(void* ptr) const;
	ENGINEMEM_API bool isAliveById(id_t id) const;

	std::string debugName;
	ENGINEMEM_API void debugWarnUnreleased() const;
	ENGINEMEM_API void debugWarnUnreleased(void* obj) const;

private:
	ENGINEMEM_API RawMemoryPool();
public:
	ENGINEMEM_API RawMemoryPool(size_t maxNumObjects, size_t objectSize);
	ENGINEMEM_API virtual ~RawMemoryPool();

	//Idiotproofing against myself
	RawMemoryPool(const RawMemoryPool&) = delete;
	RawMemoryPool(RawMemoryPool&&);

	ENGINEMEM_API void resizeObjects(size_t newSize, MemoryMapper* mapper = nullptr);

	typedef void (*hook_t)(void*);

	//Allocates raw memory.
	//Set hook if type requires special initialization
	ENGINEMEM_API void* allocate();
	hook_t initHook;

	//Deallocates raw memory.
	//Set hook if type requires special cleanup
	ENGINEMEM_API void release(void* obj);
	hook_t releaseHook;

	ENGINEMEM_API bool contains(void* ptr) const;
	inline bool isAlive(void* ptr) const
	{
		return isAliveById(ptrToId(ptr)); //Convert to id and defer to main impl
	}
	ENGINEMEM_API void reset();//doesn't reallocate memory but does reset free list and num allocated objects

	inline size_t getMaxObjectSize()  const { return mObjectSize; };
	inline size_t getNumFreeObjects() const { return mMaxNumObjects - mNumAllocatedObjects; };
	inline size_t getNumAllocatedObjects() const { return mNumAllocatedObjects; };

protected:
	void* mMemoryBlock; //One single allocation acts as both our free list and storage for objects. Free list is a dynamically-sized bitset. 1 = alive, 0 = free.
	ENGINEMEM_API uint8_t* getLivingListBlock() const { return (uint8_t*)mMemoryBlock; }
	ENGINEMEM_API void* getObjectDataBlock() const { return ((char*)mMemoryBlock)+getLivingListSpaceRequired(); }

	size_t mMaxNumObjects;
	size_t mNumAllocatedObjects;
	size_t mObjectSize;

	inline size_t getLivingListSpaceRequired() const { return ceil(mMaxNumObjects / 8.0f); }
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
