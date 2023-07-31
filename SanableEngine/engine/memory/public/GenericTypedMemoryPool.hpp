#pragma once

#include "RawMemoryPool.hpp"
#include "StableTypeInfo.hpp"

//Wrapper for TypedMemoryPool so we can still safely access common data
class GenericTypedMemoryPool : protected RawMemoryPool
{
protected:
	StableTypeInfo contentsType;

	friend class MemoryManager;

	virtual void refreshObjects(const StableTypeInfo& newTypeData, MemoryMapper* remapper) = 0;

public:
	ENGINEMEM_API GenericTypedMemoryPool(size_t maxNumObjects, size_t objectSize, StableTypeInfo contentsType);
	ENGINEMEM_API virtual ~GenericTypedMemoryPool() = default;

	ENGINEMEM_API bool isLoaded() const;
};
