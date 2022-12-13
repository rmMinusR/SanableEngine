#pragma once

#include "RawMemoryPool.hpp"

class MemoryManager;

class SafeDisposable
{
	static std::vector<SafeDisposable*>* _all;
	static std::vector<SafeDisposable*>& all();

public:
	static void disposeAll()
	{
		for (SafeDisposable* f : all()) f->disposeContents();
	}

protected:
	SafeDisposable();
	~SafeDisposable();

	virtual void disposeContents() = 0;
};

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

	void free(TObj* obj) {
		//Manual dtor call since we didn't call delete/memfree
		optional_destructor<TObj>::call(obj);

		freeRaw(obj);
	}

protected:
	void disposeContents() override
	{
		for (size_t i = 0; i < mMaxNumObjects; i++)
		{
			TObj* toFree = reinterpret_cast<TObj*>( ((uint8_t*)mMemory) + (i * mObjectSize) );
			if (std::find(mFreeList.cbegin(), mFreeList.cend(), toFree) != mFreeList.cend()) free(toFree);
		}
	}

private:
	TypedMemoryPool(const TypedMemoryPool&) = delete;
};
