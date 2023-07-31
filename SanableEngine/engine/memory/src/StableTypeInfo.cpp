#include "StableTypeInfo.hpp"

#include <cassert>

#include "GlobalTypeRegistry.hpp"

bool StableTypeInfo::isValid() const
{
	return size != 0 && name.isValid();
}

bool StableTypeInfo::isLoaded() const
{
	assert(isValid());
	return GlobalTypeRegistry::lookupType(name);
}

bool StableTypeInfo::tryRefresh()
{
	if (isLoaded())
	{
		//Overwrite values
		*this = *GlobalTypeRegistry::lookupType(name);
		return true;
	}
	else return false;
}