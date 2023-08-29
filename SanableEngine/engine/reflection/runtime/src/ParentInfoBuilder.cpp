#include "ParentInfoBuilder.hpp"

#include <cassert>

ParentInfoBuilder::ParentInfoBuilder(const TypeName& parentType, size_t ownSize, size_t parentSize, const std::function<void* (void*)>& upcastFn) :
	ownSize(ownSize),
	parentSize(parentSize),
	parent(parentType),
	upcastFn(upcastFn)
{
}

ParentInfo ParentInfoBuilder::buildFromCDOs(const std::vector<void*>& cdos) const
{
	assert(!cdos.empty());

	ParentInfo p;
	p.typeName = parent;
	p.size = parentSize;
	p.visibility = MemberVisibility::Public; //TEMP TODO make RTTI generation step

	//Detect offset using first CDO
	void* parentAddr = upcastFn(cdos[0]);
	assert( cdos[0] <= parentAddr && parentAddr < ((char*)cdos[0])+ownSize); //FIXME some implementations could theoretically allocate virtual bases on the heap. Figure it out... later.
	p.offset = size_t(((char*)parentAddr) - ((char*)cdos[0]));

	//Verify correctness against the rest
	for (int i = 1; i < cdos.size(); ++i)
	{
		assert( upcastFn(cdos[i]) == ((char*)cdos[i])+p.offset );
	}

	return p;
}

ParentInfo ParentInfoBuilder::buildFromSurrogate() const
{
	ParentInfo p;
	p.typeName = parent;
	p.size = parentSize;
	p.visibility = MemberVisibility::Public; //TEMP TODO make RTTI generation step

	//If CDOs list is empty, use surrogate instead
	assert(parentSize < 256); //TEMP measure. Can fix with multiple waves to detect
	size_t nPossibleAddrs = ownSize / sizeof(void*) + 1;
	void* surrogate[nPossibleAddrs];
	memset(surrogate, 0, parentSize);
	
	//Detect offset
	void* parentAddr = upcastFn(surrogate);
	assert( surrogate <= parentAddr && parentAddr < surrogate+nPossibleAddrs ); //FIXME some implementations could theoretically allocate virtual bases on the heap. Figure it out... later.
	p.offset = size_t(((char*)parentAddr) - ((char*)surrogate));

	return p;
}
