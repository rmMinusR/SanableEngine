#pragma once

#include "RawMemoryPool.hpp"
#include "TypeInfo.hpp"

//Wrapper for TypedMemoryPool so we can still safely access common data
class GenericTypedMemoryPool : protected RawMemoryPool
{
protected:
	TypeInfo contentsType;

public:
	ENGINEMEM_API GenericTypedMemoryPool(size_t maxNumObjects, size_t objectSize, TypeInfo contentsType);
	ENGINEMEM_API virtual ~GenericTypedMemoryPool();

	ENGINEMEM_API bool isLoaded() const;

protected:
	friend class MemoryManager;
	ENGINEMEM_API virtual void refreshObjects(const TypeInfo& newTypeData, MemoryMapper* remapper);
};
