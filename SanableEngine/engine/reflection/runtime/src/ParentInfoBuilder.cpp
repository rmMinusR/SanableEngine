#include "ParentInfoBuilder.hpp"

#include <cassert>

ParentInfoBuilder::ParentInfoBuilder(const TypeName& parentType, size_t ownSize, size_t parentSize, const std::function<void* (void*)>& upcastFn, MemberVisibility visibility, ParentInfo::Virtualness virtualness) :
	data(),
	ownSize(ownSize),
	upcastFn(upcastFn)
{
	data.size = parentSize;
	data.typeName = parentType;
	data.visibility = visibility;
	data.virtualness = virtualness;
}

ParentInfo ParentInfoBuilder::buildFromCDOs(const std::vector<void*>& cdos)
{
	assert(!cdos.empty());

	//Detect offset using first CDO
	void* parentAddr = upcastFn(cdos[0]);
	assert( cdos[0] <= parentAddr && parentAddr < ((char*)cdos[0])+ownSize); //FIXME some implementations could theoretically allocate virtual bases on the heap. Figure it out... later.
	data.offset = size_t(((char*)parentAddr) - ((char*)cdos[0]));

	//Verify correctness against the rest
	for (int i = 1; i < cdos.size(); ++i)
	{
		assert( upcastFn(cdos[i]) == ((char*)cdos[i])+data.offset );
	}

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
