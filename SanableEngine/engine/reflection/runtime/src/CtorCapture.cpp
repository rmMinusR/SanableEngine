#include "CtorCapture.hpp"

#include <cassert>
#include <variant>

#include "CapstoneWrapper.hpp"
#include "FunctionWalker.hpp"

void scanForVtables(void(*ctorThunk)())
{
	//Find last CALL. That's our ctor.
	FunctionWalker thunkWalker(ctorThunk);
	const uint8_t* ctor = nullptr;
	while (thunkWalker.advance())
	{
		if (carray_contains(thunkWalker.insn->detail->groups, thunkWalker.insn->detail->groups_count, cs_group_type::CS_GRP_CALL))
		{
			ctor = thunkWalker.codeCursor + X86_REL_ADDR(*thunkWalker.insn);
			break;
		}
	}
	assert(ctor);

	FunctionWalker ctorWalker(ctor);
	
}
