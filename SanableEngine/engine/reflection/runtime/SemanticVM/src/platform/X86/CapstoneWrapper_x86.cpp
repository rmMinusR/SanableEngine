#include "ThunkUtils.hpp"

#include "CapstoneWrapper.hpp"

void* platform_getRelAddr(const cs_insn& insn)
{
	return reinterpret_cast<void*>((uint_addr_t)X86_REL_ADDR(insn));
}

void_func_ptr platform_getRelFunc(const cs_insn& insn)
{
	return reinterpret_cast<void(*)()>((uint_addr_t)X86_REL_ADDR(insn));
}

bool platform_isIf(const cs_insn& insn)
{
	return !insn_in_group(insn, cs_group_type::CS_GRP_CALL)
		&& (insn_in_group(insn, cs_group_type::CS_GRP_BRANCH_RELATIVE) || (
				insn_in_group(insn, cs_group_type::CS_GRP_JUMP) && insn.id != X86_INS_JMP //Unconditional JMP isn't a branchable
			)
		);
}

bool platform_isInterrupt(const cs_insn& insn)
{
	return insn.id == x86_insn::X86_INS_INT3 || insn.id == x86_insn::X86_INS_INT || insn.id == x86_insn::X86_INS_INTO || insn.id == x86_insn::X86_INS_INT1;
}
