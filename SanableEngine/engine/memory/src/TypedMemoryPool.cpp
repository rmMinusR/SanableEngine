#include "TypedMemoryPool.hpp"

#include "ObjectPatch.hpp"

GenericTypedMemoryPool::GenericTypedMemoryPool(size_t maxNumObjects, const TypeInfo& contentsType) :
	RawMemoryPool(maxNumObjects, contentsType.layout.size, contentsType.layout.align),
	contentsType(contentsType),
	view(this)
{
	debugName = contentsType.name.as_str();
	releaseHook = contentsType.capabilities.rawDtor; //FIXME this will SIGSEGV on destroy if backing type isn't loaded. Switch to conditional call.
}

GenericTypedMemoryPool::~GenericTypedMemoryPool()
{

}

bool GenericTypedMemoryPool::isLoaded() const
{
	return contentsType.isLoaded();
}

const TypeInfo* GenericTypedMemoryPool::getContentsType() const
{
	return isLoaded() ? &contentsType : nullptr;
}

TypeName GenericTypedMemoryPool::getContentsTypeName() const
{
	return contentsType.name;
}

void GenericTypedMemoryPool::refreshObjects(const TypeInfo& newTypeData, MemoryMapper* remapper)
{
	assert(newTypeData.name == contentsType.name); //Ensure same type
		
	if (contentsType.isValid())
	{
		//Resize if we grew
		//Must be done before writing to members so writes don't happen in other objects' memory
		if (newTypeData.layout.size > contentsType.layout.size) resizeObjects(newTypeData.layout.size, newTypeData.layout.align, remapper);

		//Remap members and write vtable ptrs
		ObjectPatch patch = ObjectPatch::create(contentsType, newTypeData);
		for (size_t i = 0; i < mMaxNumObjects; i++)
		{
			void* obj = idToPtr(i);
			if (isAlive(obj))
			{
				patch.apply(obj, remapper);
				contentsType.layout.vptrJam(obj);
			}
		}
		
		//Resize if we shrunk
		//Must be done after writing to members so we aren't reading other objects' memory
		if (newTypeData.layout.size < contentsType.layout.size) resizeObjects(newTypeData.layout.size, newTypeData.layout.align, remapper);
	}
	
	contentsType = newTypeData;

	//Fix bad dtors
	releaseHook = newTypeData.capabilities.rawDtor;
}
