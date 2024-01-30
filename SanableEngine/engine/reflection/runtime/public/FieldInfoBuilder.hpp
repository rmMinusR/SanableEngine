#pragma once

#include <functional>
#include <string>

#include "FieldInfo.hpp"

class FieldInfoBuilder
{
private:
	TypeName declaredType;
	std::string name;
	size_t size;
	std::function<ptrdiff_t(const void*)> accessor;
	MemberVisibility visibility;

public:
	FieldInfoBuilder(const TypeName& declaredType, const std::string& name, size_t size, std::function<ptrdiff_t(const void*)> accessor, MemberVisibility visibility);

	FieldInfo build(const TypeName& ownerName, const void* ownerImage) const;
};
