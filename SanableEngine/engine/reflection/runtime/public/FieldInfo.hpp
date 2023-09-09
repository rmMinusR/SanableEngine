#pragma once

#include "MemberInfo.hpp"
#include "TypeInfo.hpp"

struct FieldInfo : public MemberInfo
{
	TypeName owner;
	TypeName type;
	std::string name;
	MemberVisibility visibility;

	ENGINE_RTTI_API void* getValue(void* objInstance) const;
	ENGINE_RTTI_API void blitValue(void* objInstance, void* value) const; //Does NOT call copy ctor! FIXME
};
