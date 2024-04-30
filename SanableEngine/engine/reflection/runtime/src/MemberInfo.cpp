#include "MemberInfo.hpp"

#include "TypeInfo.hpp"

MemberInfo::MemberInfo(size_t size, ptrdiff_t offset, const TypeName& owner) :
	size(size),
	offset(offset),
	owner(owner)
{
}

void* MemberInfo::getAddr(void* objInstance) const
{
	return ((char*)objInstance) + offset;
}

FieldInfo::FieldInfo(size_t size, ptrdiff_t offset, const TypeName& owner, const TypeName& type, const std::string& name, MemberVisibility visibility) :
	MemberInfo(size, offset, owner),
	type(type),
	name(name),
	visibility(visibility)
{
}

void* FieldInfo::getValue(void* objInstance) const
{
	return MemberInfo::getAddr(objInstance);
}

void FieldInfo::blitValue(void* objInstance, void* value) const
{
	memcpy(getAddr(objInstance), value, type.resolve()->layout.size);
}

ParentInfo::ParentInfo(size_t size, ptrdiff_t offset, const TypeName& owner, const TypeName& typeName, MemberVisibility visibility, Virtualness virtualness) :
	MemberInfo(size, offset, owner),
	typeName(typeName),
	visibility(visibility),
	virtualness(virtualness)
{
}
