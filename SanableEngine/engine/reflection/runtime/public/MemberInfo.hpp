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
	MemberVisibility visibility;
	
	//Derived classes inherit all fields of non-virtual bases, embedded as if a field were explicitly declared.
	//For virtual bases, derived classes always embed a pointer or offset (implementation-defined). When classes with any virtual base
	//are explicitly referenced by the user, the compiler also embeds inherited fields in the final version, but these inherited fields
	//are omitted from all other bases that inherit that virtual base.
	enum class Virtualness
	{
		NonVirtual = 0b01,
		VirtualExplicit = 0b11,
		VirtualInherited = 0b10
	};

	Virtualness virtualness;
};
