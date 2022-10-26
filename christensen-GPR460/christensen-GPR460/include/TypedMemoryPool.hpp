#pragma once

#include "RawMemoryPool.hpp"

//Strongly typed pointers (recommended)
template<typename TObj>
class TypedMemoryPool : protected RawMemoryPool
{
public:
	TypedMemoryPool(TypedMemoryPool&&) = default;

	template<typename... TCtorArgs>
	TObj* emplace(const TCtorArgs&... ctorArgs)
	{
		return RawMemoryPool::emplace<TObj>(ctorArgs...);
	}

	void free(TObj* obj)
	{
		RawMemoryPool::free<TObj>(obj);
	}

	TypedMemoryPool(size_t maxNumObjects) :
		RawMemoryPool(maxNumObjects, sizeof(TObj))
	{ }
private:
	TypedMemoryPool(const TypedMemoryPool&) = delete;
};

