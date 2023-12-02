#include "CtorCapture.hpp"

#include <cassert>

#include "CapstoneWrapper.hpp"
#include "FunctionBytecodeWalker.hpp"

DetectedConstants capture_utils::_captureVtablesInternal(size_t objSize, std::initializer_list<void(*)()> thunks)
{
	//Find CALLs shared by all thunks
	std::vector<void(*)()> possibleCtors;
	for (auto thunk : thunks)
	{
		//Find CALLs
		std::vector<void(*)()> calls;
		FunctionBytecodeWalker thunkWalker(thunk);
		while (thunkWalker.advance())
		{
			if (carray_contains(thunkWalker.insn->detail->groups, thunkWalker.insn->detail->groups_count, cs_group_type::CS_GRP_CALL))
			{
				calls.push_back( platform_getRelFunc(*thunkWalker.insn) );
			}
		}

		//Ensure that ctor CALL is shared by all thunks
		possibleCtors.erase(std::remove_if(
			possibleCtors.begin(),
			possibleCtors.end(),
			[&](void(*fp)())
			{
				return std::find(
					calls.begin(),
					calls.end(),
					fp
				) != calls.end();
			}
		));
	}

	//Expect one call each to the thunks calls to the same address, which will be our ctor
	assert(possibleCtors.size() == 1);
	return platform_captureConstants(objSize, possibleCtors[0]);
}
