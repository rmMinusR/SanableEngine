#pragma once

#include "ParentInfo.hpp"

#include <functional>
#include <vector>

//Internal mechanism used by TypeBuilder
//Resolving parent info late allows us to use valid objects from CDO capture step
//With normal inheritance we can use surrogates, but virtual inheritance reads additional information that's harder to fudge
class ParentInfoBuilder
{
private:
	ParentInfo data;
	size_t ownSize;
	std::function<void*(void*)> upcastFn;

public:
	ParentInfoBuilder(const TypeName& ownerType, const TypeName& parentType, size_t ownSize, size_t parentSize, const std::function<void* (void*)>& upcastFn, MemberVisibility visibility, ParentInfo::Virtualness virtualness);

	ParentInfo buildFromSurrogate(); //Only works on normal inheritance
	ParentInfo buildFromClassImage(char* image); //Works on normal and virtual inheritance
};
