#include "MemberInfo.hpp"

#include "TypeInfo.hpp"

void* InstanceMemberInfo::getAddr(void* objInstance) const
{
	return ((char*)objInstance) + offset;
}

void* FieldInfo::getValue(void* objInstance) const
{
	return InstanceMemberInfo::getAddr(objInstance);
}

void FieldInfo::blitValue(void* objInstance, void* value) const
{
	memcpy(getAddr(objInstance), value, type.resolve()->size);
}

void VTableInfo::set(void* objInstance) const
{
	memcpy(objInstance, vtable, sizeof(void*));
}
