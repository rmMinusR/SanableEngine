#pragma once

#include <type_traits>
#include <string>

#include "dllapi.h"
#include "vtable.h"

struct StableTypeInfo;

struct FieldInfo
{
	std::string name;
	size_t offset;
	StableTypeInfo* declaredType;
};

struct StableTypeInfo
{
	std::string name; //Cannot use typeinfo here
	size_t size = 0;
	vtable_ptr vtable = nullptr;

	std::vector<FieldInfo> fields;
};

using RTTIRegistry = std::vector<StableTypeInfo>;
