#pragma once

#include <type_traits>
#include <typeinfo>
#include <string>
#include <vector>

#include "dllapi.h"
#include "vtable.h"

class TypeInfo;
struct FieldInfo;

struct Module
{
	std::vector<TypeInfo> types;

	ENGINEREFL_API Module();
	ENGINEREFL_API Module(std::initializer_list<TypeInfo>);
	ENGINEREFL_API ~Module();
};
