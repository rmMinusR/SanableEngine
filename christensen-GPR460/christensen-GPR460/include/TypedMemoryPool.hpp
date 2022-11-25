#pragma once

#include "RawMemoryPool.hpp"

class MemoryManager;

class SafeDisposable
{
public:
	static void disposeAll();

protected:
	SafeDisposable();
	~SafeDisposable();

	virtual void disposeContents() = 0;
};
constexpr size_t allSafeDisposablesCount = 256;
extern SafeDisposable** allSafeDisposables;

//Strongly typed pointers (recommended)
template<typename TObj>
class TypedMemoryPool : protected RawMemoryPool, protected SafeDisposable
{
public:
	TypedMemoryPool(size_t maxNumObjects) :
		RawMemoryPool(maxNumObjects, sizeof(TObj))
	{ }
	TypedMemoryPool(TypedMemoryPool&&) = default;

	//Allocates memory and creates an object. RECOMMENDED because it has decent
	//type safety and data safety.
	template<typename... TCtorArgs>
	TObj* emplace(TCtorArgs... ctorArgs) {
		assert(sizeof(TObj) <= mObjectSize);
		TObj* pObj = reinterpret_cast<TObj*>(allocateRaw());
		assert(pObj);
		//Construct object
		new (pObj) TObj(ctorArgs...);
		return pObj;
	}

	virtual void freeSafe(void* obj) override
	{
		//Manual dtor call since we didn't call delete/memfree
		optional_destructor<TObj>::call(reinterpret_cast<TObj*>(obj));

		freeRaw(obj);
	}

protected:
	void disposeContents() override
	{
		for (size_t i = 0; i < mMaxNumObjects; i++)
		{
			TObj* toFree = reinterpret_cast<TObj*>( ((uint8_t*)mMemory) + (i * mObjectSize) );
			if (std::find(mFreeList.cbegin(), mFreeList.cend(), toFree) != mFreeList.cend()) freeSafe(toFree);
		}
		mFreeList.clear();
		createFreeList();
	}

private:
	TypedMemoryPool(const TypedMemoryPool&) = delete;
};
