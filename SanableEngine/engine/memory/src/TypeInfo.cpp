#include "TypeInfo.hpp"

#include <cassert>

#include "GlobalTypeRegistry.hpp"

bool TypeInfo::isValid() const
{
	return size != 0 && name.isValid();
}

bool TypeInfo::isLoaded() const
{
	assert(isValid());
	return GlobalTypeRegistry::lookupType(name);
}

bool TypeInfo::tryRefresh()
{
	if (isLoaded())
	{
		//Overwrite values
		*this = *GlobalTypeRegistry::lookupType(name);
		return true;
	}
	else return false;
}
