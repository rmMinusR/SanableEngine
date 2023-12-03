#include "CtorCapture.hpp"

#include <cassert>

#include "CapstoneWrapper.hpp"
#include "FunctionBytecodeWalker.hpp"

DetectedConstants capture_utils::_captureVtablesInternal(size_t objSize, std::initializer_list<void(*)()> thunks, std::initializer_list<void(*)()> blacklist, bool* wasBlacklistTripped_out_optional)
{
	/// OUTLINE:
	/// 1. Find CALLs present in any thunk
	/// 2. Filter only CALLs that appear in all thunks
	/// 3. Only ctor will remain

	//1. Find CALLs present in any thunk
	std::vector<void(*)()> possibleCtors;
	for (auto thunk : thunks)
	{
		FunctionBytecodeWalker thunkWalker(thunk);
		while (thunkWalker.advance())
		{
			if (carray_contains(thunkWalker.insn->detail->groups, thunkWalker.insn->detail->groups_count, cs_group_type::CS_GRP_CALL))
			{
				void(*fp)() = platform_getRelFunc(*thunkWalker.insn);
				auto it = std::find(possibleCtors.begin(), possibleCtors.end(), fp);
				if (it == possibleCtors.end()) possibleCtors.push_back(fp);
			}
		}
	}
	
	assert(possibleCtors.size() > 0);

	//2. Filter only CALLs that appear in all thunks
	for (auto thunk : thunks)
	{
		//Find CALLs not present in this thunk
		std::vector<void(*)()> notPresent = possibleCtors;
		FunctionBytecodeWalker thunkWalker(thunk);
		while (thunkWalker.advance())
		{
			if (carray_contains(thunkWalker.insn->detail->groups, thunkWalker.insn->detail->groups_count, cs_group_type::CS_GRP_CALL))
			{
				void(*fp)() = platform_getRelFunc(*thunkWalker.insn);
				auto it = std::find(notPresent.begin(), notPresent.end(), fp);
				if (it != notPresent.end()) notPresent.erase(it);
			}
		}

		//Remove calls not present from the list of possible ctors
		for (void(*i)() : notPresent) possibleCtors.erase(std::find(possibleCtors.begin(), possibleCtors.end(), i));
	}

	//Some compilers will pre-zero, especially in debug mode. Allow us to not catch that.
	for (void(*i)() : blacklist)
	{
		auto it = std::find(possibleCtors.begin(), possibleCtors.end(), i);
		bool found = (it != possibleCtors.end());
		if (found) possibleCtors.erase(it);
		if (wasBlacklistTripped_out_optional) *wasBlacklistTripped_out_optional = found;
	}

	//3. Expect one call each to the thunks calls to the same address, which will be our ctor
	assert(possibleCtors.size() == 1);
	return DetectedConstants::captureCtor(objSize, possibleCtors[0]);
}
