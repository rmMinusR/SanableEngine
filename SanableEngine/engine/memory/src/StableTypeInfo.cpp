#include "StableTypeInfo.inl"

#include <cassert>

bool StableTypeInfo::isValid() const
{
	return size != 0 && name.isValid();
}

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
