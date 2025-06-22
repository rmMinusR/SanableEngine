#include "SAny.hpp"

#include <cassert>

#include "TypeInfo.hpp"

void* stix::SAnyRef::get_internal(const TypeName& asType) const
{
	if (!data) return nullptr;

	assert(asType == getTypeName());
	return data;
}

stix::SAnyRef::SAnyRef(void* data, const TypeName& type) :
	data(data),
	type(type)
{
	assert(data);
	assert(type.isValid());
}

stix::SAnyRef::SAnyRef(void* data, const TypeInfo& type) :
	data(data),
	type(&type)
{
	assert(data);
	assert(type.isValid());
}

stix::SAnyRef::SAnyRef() :
	data(nullptr),
	type()
{
}

stix::SAnyRef::~SAnyRef()
{
}

TypeName stix::SAnyRef::getTypeName() const
{
	if (std::holds_alternative<TypeName>(type))
	{
		return std::get<TypeName>(type);
	}
	else if (std::holds_alternative<const TypeInfo*>(type))
	{
		return std::get<const TypeInfo*>(type)->name;
	}
	else
	{
		assert(false); // Invalid
		return TypeName();
	}
}

const TypeInfo* stix::SAnyRef::tryGetTypeInfo() const
{
	assert(has_value());
	if (std::holds_alternative<const TypeInfo*>(type))
	{
		return std::get<const TypeInfo*>(type);
	}
	else return nullptr;
}

stix::SAnyRef::operator bool() const
{
	return data;

}

bool stix::SAnyRef::has_value() const
{
	return data;
}
