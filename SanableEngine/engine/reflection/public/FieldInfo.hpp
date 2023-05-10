#pragma once

#include <type_traits>
#include <typeinfo>
#include <string>
#include <vector>

#include "dllapi.h"
#include "vtable.h"

class TypeInfo;
struct FieldInfo;

struct FieldInfo
{
	std::string name;
	size_t offset;
	TypeInfo* declaredType;
	bool isPointer; //Pointer-to-pointer is unsupported, for now

	ENGINEREFL_API FieldInfo() = default;
	ENGINEREFL_API ~FieldInfo() = default;

	ENGINEREFL_API bool operator==(const FieldInfo& other) const;
	ENGINEREFL_API bool operator!=(const FieldInfo& other) const;
};
