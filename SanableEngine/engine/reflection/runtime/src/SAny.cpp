#include "SAny.hpp"

#include <cassert>

void* SAnyRef::get_internal(const TypeName& asType) const
{
	if (!data) return nullptr;

	assert(asType == type);
	return data;
}

SAnyRef::SAnyRef(void* data, const TypeName& type) :
	data(data),
	type(type)
{
	assert(data);
	assert(type.isValid());
}

SAnyRef::SAnyRef() :
	data(nullptr),
	type()
{
}

SAnyRef::~SAnyRef()
{
}

TypeName SAnyRef::getType() const
{
	return type;
}

SAnyRef::operator bool() const
{
	return data;

}

bool SAnyRef::has_value() const
{
	return data;
}
