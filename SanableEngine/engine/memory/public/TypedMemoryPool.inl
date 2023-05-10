#pragma once

#include "RawMemoryPool.hpp"

#include <stdio.h>

#include "RTTIRegistry.hpp"

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
	TypeInfo* hotswap;

public:
	TypedMemoryPool(size_t maxNumObjects = PoolSettings<TObj>::maxObjectCount) :
		RawMemoryPool(maxNumObjects, sizeof(TObj)),
		hotswap(RTTIRegistry::get()->lookupType(typeid(TObj)))
	{
		releaseHook = (RawMemoryPool::hook_t) optional_destructor<TObj>::call;
		//FIXME need a way to obtain TypeInfo
	}

	void refreshVtables(const std::vector<TypeInfo*>& refreshers) override
	{
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

	//Allocates memory and creates an object.
	template<typename... TCtorArgs>
	TObj* emplace(TCtorArgs... ctorArgs)
	{
		//Allocate memory
		TObj* pObj = reinterpret_cast<TObj*>(allocate());
		assert(pObj);

		//Construct object
		new (pObj) TObj(ctorArgs...);

		return pObj;
	}

	//Pass through
	inline void release(TObj* obj) { RawMemoryPool::release(obj); }
protected:

	TypedMemoryPool(TypedMemoryPool&&) = default;
	TypedMemoryPool(const TypedMemoryPool&) = delete;
};
