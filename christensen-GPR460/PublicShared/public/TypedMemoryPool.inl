#pragma once

#include "RawMemoryPool.hpp"
#include "Hotswap.hpp"

#include <stdio.h>

template<typename T, size_t _maxObjectCount = 32>
struct PoolSettings
{
public:
	constexpr static size_t maxObjectCount = _maxObjectCount;
};

//Strongly typed pointers (recommended)
template<typename TObj>
class TypedMemoryPool : protected RawMemoryPool
{
	HotswapTypeData hotswap;

public:
	TypedMemoryPool(size_t maxNumObjects = PoolSettings<TObj>::maxObjectCount) :
		RawMemoryPool(maxNumObjects, sizeof(TObj)),
		hotswap(HotswapTypeData::blank<TObj>())
	{
		releaseHook = (RawMemoryPool::hook_t) optional_destructor<TObj>::call;
	}

	void refreshVtables(const std::vector<HotswapTypeData*>& refreshers) override
	{
		auto newHotswap = std::find_if(refreshers.cbegin(), refreshers.cend(), [&](HotswapTypeData* d) { return d->name == hotswap.name; });
		if (newHotswap != refreshers.cend())
		{
			assert(hotswap.name == (**newHotswap).name); //Ensure same type
			assert(hotswap.size == (**newHotswap).size); //Ensure no changes to data members (FIXME fragile)
			hotswap = **newHotswap;

			for (size_t i = 0; i < mMaxNumObjects; i++)
			{
				TObj* obj = reinterpret_cast<TObj*>(((uint8_t*)mMemory) + (i * mObjectSize));
				bool isAlive = std::find(mFreeList.cbegin(), mFreeList.cend(), obj) == mFreeList.cend();
				if (isAlive) set_vtable_ptr(obj, hotswap.vtable);
			}
		}
	}

	//Allocates memory and creates an object.
	template<typename... TCtorArgs>
	TObj* emplace(TCtorArgs... ctorArgs)
	{
		//Allocate memory
		TObj* pObj = reinterpret_cast<TObj*>(allocate());
		assert(pObj);

		//Construct object
		new (pObj) TObj(ctorArgs...);

		//Extract vtable (if not initted)
		if (!hotswap.vtable) hotswap.set_vtable(pObj);

		return pObj;
	}

	//Pass through
	inline void release(TObj* obj) { RawMemoryPool::release(obj); }
protected:

	TypedMemoryPool(TypedMemoryPool&&) = default;
	TypedMemoryPool(const TypedMemoryPool&) = delete;
};
