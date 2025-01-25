#include "SAny.hpp"

#include <cassert>

#include "TypeInfo.hpp"

stix::_OwningRefImpl::_OwningRefImpl()
{
}

stix::_OwningRefImpl::~_OwningRefImpl()
{
	clear();
}

void stix::_OwningRefImpl::clear()
{
	if (obj)
	{
		assert(deleter != nullptr);
		deleter(obj);
	}
	obj = nullptr;
	deleter = nullptr;

	if (ownsTypeInfo && objType) delete objType;
	objType = nullptr;
	ownsTypeInfo = false;
}

void stix::_OwningRefImpl::put(void* obj, void(*deleter)(void*), const TypeInfo* typeInfo, bool ownsTypeInfo)
{
	if (this->obj) clear();

	this->obj = obj;
	this->deleter = deleter;
	this->objType = typeInfo;
	this->ownsTypeInfo = ownsTypeInfo;
}

stix::_OwningRefImpl::_OwningRefImpl(_OwningRefImpl&& mov)
{
	*this = std::move(mov);
}

stix::_OwningRefImpl& stix::_OwningRefImpl::operator=(_OwningRefImpl&& mov)
{
	put(mov.obj, mov.deleter, mov.objType, mov.ownsTypeInfo);
	mov.clear();

	return *this;
}

stix::_OwningRefImpl::operator bool() const
{
	return valid();
}

bool stix::_OwningRefImpl::valid() const
{
	return obj != nullptr;
}

const TypeInfo* stix::_OwningRefImpl::type() const
{
	return objType;
}

void* stix::_OwningRefImpl::try_get_as(const TypeName& requestedTypeName)
{
	if (objType->name == requestedTypeName) return obj;

	// Try upcast
	void* out = objType->upcast(obj, requestedTypeName);
	if (out) return out;

	// Try downcast
	const TypeInfo* requestedType = requestedTypeName.resolve();
	if (requestedType)
	{
		return requestedType->downcast(obj, objType->name);
	}

	// FIXME should this fail loudly?
	return nullptr;
}
