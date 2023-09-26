#pragma once

#include "RawMemoryPool.hpp"
#include "TypeInfo.hpp"

//Wrapper for TypedMemoryPool so we can still safely access common data
class GenericTypedMemoryPool : protected RawMemoryPool
{
protected:
	TypeInfo contentsType;
	friend class MemoryManager;

public:
	ENGINEMEM_API GenericTypedMemoryPool(size_t maxNumObjects, const TypeInfo& contentsType);
	ENGINEMEM_API ~GenericTypedMemoryPool();

	ENGINEMEM_API bool isLoaded() const;

	inline size_t getNumFreeObjects     () const { return RawMemoryPool::getNumFreeObjects(); }
	inline size_t getNumAllocatedObjects() const { return RawMemoryPool::getNumAllocatedObjects(); }

	//INTERNAL USE ONLY
	ENGINEMEM_API void refreshObjects(const TypeInfo& newTypeData, MemoryMapper* remapper);
};
