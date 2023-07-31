#include "GenericTypedMemoryPool.hpp"

GenericTypedMemoryPool::GenericTypedMemoryPool(size_t maxNumObjects, size_t objectSize, StableTypeInfo contentsType) :
	RawMemoryPool(maxNumObjects, objectSize),
	contentsType(contentsType)
{
}

bool GenericTypedMemoryPool::isLoaded() const
{
	return contentsType.isLoaded();
}
