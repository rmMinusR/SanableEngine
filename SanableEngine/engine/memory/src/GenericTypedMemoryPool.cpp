#include "GenericTypedMemoryPool.hpp"

GenericTypedMemoryPool::GenericTypedMemoryPool(size_t maxNumObjects, size_t objectSize, TypeInfo contentsType) :
	RawMemoryPool(maxNumObjects, objectSize),
	contentsType(contentsType)
{
	debugName = contentsType.name.as_str();
}

GenericTypedMemoryPool::~GenericTypedMemoryPool()
{

}

bool GenericTypedMemoryPool::isLoaded() const
{
	return contentsType.isLoaded();
}
