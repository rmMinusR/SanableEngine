#include "TypedMemoryPool.hpp"

#include "ObjectPatch.hpp"

GenericTypedMemoryPool::GenericTypedMemoryPool(size_t maxNumObjects, size_t objectSize, const TypeInfo& contentsType) :
	RawMemoryPool(maxNumObjects, objectSize),
	contentsType(contentsType),
	view(this)
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

void GenericTypedMemoryPool::refreshObjects(const TypeInfo& newTypeData, MemoryMapper* remapper)
{
	assert(newTypeData.name == contentsType.name); //Ensure same type
		
	if (contentsType.isValid())
	{
		//Resize if we grew
		//Must be done before writing to members so writes don't happen in other objects' memory
		if (newTypeData.size > contentsType.size) resizeObjects(newTypeData.size, remapper);

		//Remap members and write vtable ptrs
		ObjectPatch patch = ObjectPatch::create(contentsType, newTypeData);
		for (size_t i = 0; i < mMaxNumObjects; i++)
		{
			void* obj = idToPtr(i);
			if (isAlive(obj))
			{
				patch.apply(obj, remapper);
				contentsType.vptrJam(obj);
			}
		}
		
		//Resize if we shrunk
		//Must be done after writing to members so we aren't reading other objects' memory
		if (newTypeData.size < contentsType.size) resizeObjects(newTypeData.size, remapper);
	}
	
	contentsType = newTypeData;

	//Fix bad dtors
	releaseHook = newTypeData.dtor;
}
