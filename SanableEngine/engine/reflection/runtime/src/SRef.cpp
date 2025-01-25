#include "SRef.hpp"

#include <cassert>

void* stix::SRef::get_internal(const TypeName& asType) const
{
	if (!data) return nullptr;

	assert(asType == type);
	return data;
}

stix::SRef::SRef(void* data, const TypeName& type) :
	data(data),
	type(type)
{
	assert(data);
	assert(type.isValid());
}

stix::SRef::SRef() :
	data(nullptr),
	type()
{
}

stix::SRef::~SRef()
{
}

TypeName stix::SRef::getType() const
{
	return type;
}

stix::SRef::operator bool() const
{
	return data;

}

bool stix::SRef::has_value() const
{
	return data;
}
