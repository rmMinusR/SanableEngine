#include "ParentInfoBuilder.hpp"

#include <cassert>

#include "alloc_detail.h"

ParentInfoBuilder::ParentInfoBuilder(const TypeName& ownerType, const TypeName& parentType, size_t ownerSize, size_t parentSize, const std::function<void* (void*)>& upcastFn, MemberVisibility visibility, ParentInfo::Virtualness virtualness) :
	data(parentSize, -1, ownerType, parentType, visibility, virtualness),
	ownerSize(ownerSize),
	upcastFn(upcastFn)
{
}

ParentInfo ParentInfoBuilder::buildFromClassImage(char* image)
{
	if (!image && data.virtualness == ParentInfo::Virtualness::NonVirtual) image = reinterpret_cast<char*>(0xDEADBEEFull);
	assert(image != nullptr && "Casting requires class image, but none was provided!");

	//Detect offset
	void* parentAddr = upcastFn(image);
	data.offset = size_t( ((char*)parentAddr) - image );
	assert( 0 <= data.offset && data.offset < ownerSize ); //FIXME some implementations could theoretically allocate virtual bases on the heap. Figure it out... later.

	return data;
}

ParentInfo ParentInfoBuilder::buildFromSurrogate()
{
	//If CDOs list is empty, use surrogate instead
	char* surrogate = (char*) STACK_ALLOC(ownerSize);
	memset(surrogate, 0, ownerSize);
	
	//Detect offset
	void* parentAddr = upcastFn(surrogate);
	assert( surrogate <= parentAddr && parentAddr < surrogate+ownerSize ); //FIXME some implementations could theoretically allocate virtual bases on the heap. Figure it out... later.
	data.offset = size_t(((char*)parentAddr) - ((char*)surrogate));

	STACK_FREE(surrogate);
	return data;
}
