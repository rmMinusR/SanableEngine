#include "MemberInfo.hpp"

#include "TypeInfo.hpp"

void* MemberInfo::getAddr(void* objInstance) const
{
	return ((char*)objInstance) + offset;
}

void* FieldInfo::getValue(void* objInstance) const
{
	return MemberInfo::getAddr(objInstance);
}

void FieldInfo::blitValue(void* objInstance, void* value) const
{
	memcpy(getAddr(objInstance), value, type.resolve()->size);
}
