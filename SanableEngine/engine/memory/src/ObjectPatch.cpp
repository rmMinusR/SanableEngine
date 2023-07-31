#include "ObjectPatch.hpp"

#include <cassert>

void ObjectPatch::apply(void* target, MemoryMapper* remapLog) const
{
	if (oldData.isValid() && newData.isValid())
	{
		//TODO implement
	}
}

bool ObjectPatch::isValid() const
{
	return oldData.isValid() || newData.isValid();
}

TypeName ObjectPatch::getTypeName() const
{
	assert(isValid());
	if (oldData.isValid()) return oldData.name;
	else                   return newData.name;
}

void ObjectPatch::debugLog() const
{
	assert(isValid());

	if (oldData.isValid() && !newData.isValid()) printf("%s has gone missing!", oldData.name.c_str());
	else if (!oldData.isValid() && newData.isValid()) printf("%s is new!", newData.name.c_str());
	else
	{
		//TODO print member diff
	}
}

ObjectPatch ObjectPatch::create(StableTypeInfo oldData, StableTypeInfo newData)
{
	ObjectPatch out;
	out.oldData = oldData;
	out.newData = newData;
	return out;
}
