#pragma once

#include "TypeName.hpp"

struct InstanceMemberInfo
{
	size_t size;
	size_t offset;

protected:
	ENGINE_RTTI_API void* getAddr(void* objInstance) const;
};

enum class MemberVisibility
{
	Private   = 1<<0,
	Protected = 1<<1,
	Public    = 1<<2,

	All = ~0
};


struct FieldInfo : public InstanceMemberInfo
{
	TypeName owner;
	TypeName type;
	std::string name;
	MemberVisibility visibility;

	ENGINE_RTTI_API void* getValue(void* objInstance) const;
	ENGINE_RTTI_API void blitValue(void* objInstance, void* value) const; //Does NOT call copy ctor! FIXME
};

struct ParentInfo : public InstanceMemberInfo
{
	TypeName typeName;
	bool isVirtual;
	MemberVisibility visibility;
};
