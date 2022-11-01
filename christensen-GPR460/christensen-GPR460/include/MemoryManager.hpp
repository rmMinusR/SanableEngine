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

	template<typename TObj>
	static void destroy(TObj* obj)
	{
		PoolInstance<TObj>::pool.free(obj);
	}

	static void init();
	static void cleanup();
};
