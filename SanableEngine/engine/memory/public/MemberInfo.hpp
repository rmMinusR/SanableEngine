#pragma once

#include <variant>

#include "TypeName.hpp"

struct InstanceMemberInfo
{
	size_t offset;

protected:
	ENGINEMEM_API void* getAddr(void* objInstance) const;
};

enum class MemberVisibility
{
	Private   = 1<<0,
	Protected = 1<<1,
	Public    = 1<<2,
};


struct FieldInfo : public InstanceMemberInfo
{
	TypeName type;
	std::string name;
	MemberVisibility visibility;

	ENGINEMEM_API void* getValue(void* objInstance) const;
	ENGINEMEM_API void blitValue(void* objInstance, void* value) const; //Does NOT call copy ctor! FIXME
};

struct VTableInfo : public InstanceMemberInfo
{
	void* vtable;

	ENGINEMEM_API void set(void* objInstance) const;
};

struct ParentInfo : public InstanceMemberInfo
{
	TypeName typeName;
	MemberVisibility visibility;
};
