#include "FunctionWalker.hpp"

#include "CapstoneWrapper.hpp"

FunctionWalker::FunctionWalker(void(*fn)())
{
	codeCursor = reinterpret_cast<uint8_t*>(fn);
	insn = cs_malloc(capstone_get_instance());
	furthestKnownJump = nullptr;
}

FunctionWalker::~FunctionWalker()
{
	cs_free(insn, 1);
	insn = nullptr;
}

bool FunctionWalker::advance()
{
	//Advance cursor and interpret next
	uint64_t addr = (uint64_t)reinterpret_cast<uint_addr_t>(codeCursor);
	size_t allowedToProcess = 32; //No way to know for sure, but we can do some stuff with JUMP/RET detection to figure it out
	cs_disasm_iter(capstone_get_instance(), &codeCursor, &allowedToProcess, &addr, insn);

	//If it's a JUMP, record that we can go further in this function
	if (carray_contains(insn->detail->groups, insn->detail->groups_count, cs_group_type::CS_GRP_JUMP)
	 || carray_contains(insn->detail->groups, insn->detail->groups_count, cs_group_type::CS_GRP_BRANCH_RELATIVE))
	{
		const uint8_t* jumpLoc = codeCursor + X86_REL_ADDR(*insn);
		if (jumpLoc > furthestKnownJump) furthestKnownJump = jumpLoc;
		return true;
	}
	//If it's a RETURN, continue only if we are allowed to JUMP to later
	else if (carray_contains(insn->detail->groups, insn->detail->groups_count, cs_group_type::CS_GRP_RET))
	{
		return furthestKnownJump > codeCursor;
	}
	//Normal instructions: just continue parsing
	else return true;
}
