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

	//Allocates raw memory. NOT RECOMMENDED because we can't find the size of
	//a void pointer, meaning this must be tracked separately for us to find
	//the right memory pool within reasonable time.
	void* allocateRaw();

	//Deallocates raw memory. NOT RECOMMENDED because we can't find the size of
	//a void pointer, meaning this must be tracked separately for us to find
	//the right memory pool within reasonable time. Also not recommended because
	//it doesn't call destructor.
	void freeRaw(void* obj);

	//Allocates memory and creates an object. RECOMMENDED because it has decent
	//type safety and data safety.
	template<typename TObj, typename... TCtorArgs>
	TObj* emplace(const TCtorArgs&... ctorArgs) {
		assert(sizeof(TObj) <= mObjectSize);
		TObj* pObj = reinterpret_cast<TObj*>(allocateRaw());
		assert(pObj);
		//Construct object
		new (pObj) TObj(ctorArgs...);
		return pObj;
	}

	template<typename TObj>
	void free(TObj* obj) {
		//Manual dtor call since we didn't call delete/memfree
		optional_destructor<TObj>::call(obj);

		freeRaw(obj);
	}

	bool contains(void* ptr) const;
	void reset();//doesn't reallocate memory but does reset free list and num allocated objects

	inline size_t getMaxObjectSize()  const { return mObjectSize; };
	inline size_t getNumFreeObjects() const { return mMaxNumObjects - mNumAllocatedObjects; };
	inline size_t getNumAllocatedObjects() const { return mNumAllocatedObjects; };
private:

	void* mMemory;
	void* mHighestValidAddress;
	size_t mMaxNumObjects;
	size_t mNumAllocatedObjects;
	size_t mObjectSize;
	std::vector<void*> mFreeList;

	void createFreeList();
};
