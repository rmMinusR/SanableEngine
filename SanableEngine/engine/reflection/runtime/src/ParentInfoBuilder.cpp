#include "ParentInfoBuilder.hpp"

#include <cassert>

ParentInfoBuilder::ParentInfoBuilder(const TypeName& ownerType, const TypeName& parentType, size_t ownSize, size_t parentSize, const std::function<void* (void*)>& upcastFn, MemberVisibility visibility, ParentInfo::Virtualness virtualness) :
	data(parentSize, -1, ownerType, parentType, visibility),
	ownSize(ownSize),
	upcastFn(upcastFn)
{
}

ParentInfo ParentInfoBuilder::buildFromClassImage(char* image)
{
	assert((data.virtualness == ParentInfo::Virtualness::NonVirtual || image != nullptr) && "Casting requires class image, but none was provided!");

	//Detect offset
	void* parentAddr = upcastFn(image);
	data.offset = size_t( ((char*)parentAddr) - image );
	assert( 0 <= data.offset && data.offset < ownSize ); //FIXME some implementations could theoretically allocate virtual bases on the heap. Figure it out... later.

	return data;
}

ParentInfo ParentInfoBuilder::buildFromSurrogate()
{
	//If CDOs list is empty, use surrogate instead
	char* surrogate[ownSize];
	memset(surrogate, 0, ownSize);
	
	//Detect offset
	void* parentAddr = upcastFn(surrogate);
	assert( surrogate <= parentAddr && parentAddr < surrogate+ownSize ); //FIXME some implementations could theoretically allocate virtual bases on the heap. Figure it out... later.
	data.offset = size_t(((char*)parentAddr) - ((char*)surrogate));

	return data;
}
