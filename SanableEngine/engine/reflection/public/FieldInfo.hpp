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
	std::ptrdiff_t offset;
	TypeInfo* declaredType;
	TypeInfo* owner;
	bool isPointer; //Pointer-to-pointer is unsupported, for now

	ENGINEREFL_API FieldInfo() = default;
	ENGINEREFL_API ~FieldInfo() = default;

	ENGINEREFL_API bool operator==(const FieldInfo& other) const;
	ENGINEREFL_API bool operator!=(const FieldInfo& other) const;

	ENGINEREFL_API void* bind(void* obj) const;
};
