#include "FieldInfo.hpp"

bool FieldInfo::operator==(const FieldInfo& other) const
{
	return this->name == other.name;
}

bool FieldInfo::operator!=(const FieldInfo& other) const
{
	return !(*this == other);
}

void* FieldInfo::bind(void* obj) const
{
	return ((char*)obj)+this->offset;
}