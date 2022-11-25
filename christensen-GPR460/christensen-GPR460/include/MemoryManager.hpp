#pragma once

#include <vector>
#include "TypedMemoryPool.hpp"

class MemoryManager
{
private:
	constexpr static size_t maxObjectCount = 32;

	template<typename T>
	struct PoolInstance
	{
	public:
		static inline TypedMemoryPool<T> pool = TypedMemoryPool<T>(maxObjectCount);
	};

public:
	template<typename TObj, typename... TCtorArgs>
	static TObj* create(TCtorArgs... ctorArgs)
	{
		return PoolInstance<TObj>::pool.emplace(ctorArgs...);
	}

	static void destroy_wide(void* obj); //Destroy an object by searching all memory pools and choosing the right one

	template<typename TObj>
	static void destroy_narrow(TObj* obj) //If we already know the type, jump to the pool and destroy
	{
		PoolInstance<TObj>::pool.freeSafe(obj);
	}

	static void init();
	static void cleanup();
};
