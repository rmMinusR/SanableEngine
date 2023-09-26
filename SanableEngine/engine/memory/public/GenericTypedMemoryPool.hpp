#pragma once

#include "RawMemoryPool.hpp"
#include "TypeInfo.hpp"

//Wrapper for TypedMemoryPool so we can still safely access common data
class GenericTypedMemoryPool : protected RawMemoryPool
{
protected:
	TypeInfo contentsType;

public:
	ENGINEMEM_API GenericTypedMemoryPool(size_t maxNumObjects, size_t objectSize, const TypeInfo& contentsType);
	ENGINEMEM_API ~GenericTypedMemoryPool();

	ENGINEMEM_API bool isLoaded() const;

	inline size_t getNumFreeObjects     () const { return RawMemoryPool::getNumFreeObjects(); }
	inline size_t getNumAllocatedObjects() const { return RawMemoryPool::getNumAllocatedObjects(); }

protected:
	friend class MemoryManager;
	ENGINEMEM_API void refreshObjects(const TypeInfo& newTypeData, MemoryMapper* remapper);
};
