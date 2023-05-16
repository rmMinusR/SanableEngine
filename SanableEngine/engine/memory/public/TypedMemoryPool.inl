#pragma once

#include "RawMemoryPool.hpp"

#include <stdio.h>

#include "RTTIRegistry.hpp"

/// 
/// Remap algorithm:
/// 
/// Handle living object instances - RECORD ALL MOVES
///		Pooled object shuffling if size changed
///		Pooled object member shuffling
/// 
/// Calculate DLL remap
///		Calculate new root address
///		Calculate new relative addresses for all symbols
/// 
/// Update pointers referencing remapped memory (living objects or DLLs)
/// 

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
public:
	TypedMemoryPool(size_t maxNumObjects = PoolSettings<TObj>::maxObjectCount) :
		RawMemoryPool(maxNumObjects, sizeof(TObj))
	{
		releaseHook = (RawMemoryPool::hook_t) optional_destructor<TObj>::call;
		dataType = RTTIRegistry::get()->lookupType(typeid(TObj));
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
