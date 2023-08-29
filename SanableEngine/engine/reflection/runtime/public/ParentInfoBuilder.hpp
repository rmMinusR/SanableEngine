#pragma once

#include "TypeName.hpp"
#include "MemberInfo.hpp"

#include <functional>

//Internal mechanism used by TypeBuilder
//Resolving parent info late allows us to use valid objects from CDO capture step
//With normal inheritance, we could use surrogates, but virtual inheritance reads additional information that's harder to fudge
class ParentInfoBuilder
{
private:
	size_t ownSize;
	size_t parentSize;
	TypeName parent;
	std::function<void* (void*)> upcastFn;

public:
	ParentInfoBuilder(const TypeName& parentType, size_t ownSize, size_t parentSize, const std::function<void* (void*)>& upcastFn);

	ParentInfo buildFromSurrogate() const; //Only works on normal inheritance
	ParentInfo buildFromCDOs(const std::vector<void*>& cdos) const; //Works on normal and virtual inheritance
};
