#pragma once

#include "TypeName.hpp"

struct InstanceMemberInfo
{
	size_t offset;

protected:
	ENGINEMEM_API void* getAddr(void* objInstance) const;
};

struct FieldInfo : public InstanceMemberInfo
{
	TypeName type;
	std::string name;

	ENGINEMEM_API void* getValue(void* objInstance) const;
	ENGINEMEM_API void blitValue(void* objInstance, void* value) const; //Does NOT call copy ctor! FIXME
};

struct VTableInfo : public InstanceMemberInfo
{
	void* vtable;

	ENGINEMEM_API void set(void* objInstance) const;
};
