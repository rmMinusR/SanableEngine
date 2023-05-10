#pragma once

#include <typeinfo>

class TypeInfo;

class RTTIRegistry
{
public:
	inline static RTTIRegistry* get() { return nullptr; } //TODO implement stub

	inline TypeInfo* lookupType(const std::type_info&) { return nullptr; } //TODO implement stub
};
