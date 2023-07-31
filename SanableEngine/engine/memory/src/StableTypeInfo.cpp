#include "StableTypeInfo.hpp"

#include <cassert>

bool StableTypeInfo::isValid() const
{
	return size != 0 && name.isValid();
}
