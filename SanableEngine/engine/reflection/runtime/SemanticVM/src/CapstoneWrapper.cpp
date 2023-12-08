#include "CapstoneWrapper.hpp"

#include <cassert>

csh capstone_instance;

void capstone_check_error(cs_err code)
{
	if (code != CS_ERR_OK)
	{
		printf("Error with Capstone: %s\n", cs_strerror(code));
		assert(false);
	}
}

csh capstone_get_instance()
{
	assert(cs_support(cs_arch::CS_ARCH_OURS));

	if (!capstone_instance)
	{
		cs_err status = cs_open(
			cs_arch::CS_ARCH_OURS,
			sizeof(void*) == 8 ? cs_mode::CS_MODE_64 : cs_mode::CS_MODE_32,
			&capstone_instance
		);
		capstone_check_error(status);

		status = cs_option(capstone_instance, cs_opt_type::CS_OPT_DETAIL, cs_opt_value::CS_OPT_ON);
		capstone_check_error(status);
	}

	return capstone_instance;
}

void capstone_cleanup_instance()
{
	if (capstone_instance)
	{
		cs_err status = cs_close(&capstone_instance);
		capstone_check_error(status);
	}
}


bool insn_in_group(const cs_insn& insn, uint8_t group)
{
	return carray_contains(insn.detail->groups, insn.detail->groups_count, group);
}


int printInstructionCursor(const cs_insn* insn, int callLevel)
{
	int bytesWritten = 0;
	for (int i = 0; i < callLevel   ; ++i) bytesWritten += printf(" |  "); //Indent
	                                       bytesWritten += printf("%p: ", (void*)insn->address); //Write address
	//for (int i = 0; i < insn->size  ; ++i) bytesWritten += printf(" %02x", insn->bytes[i]); //Write raw bytes
	//for (int i = 0; i < 8-insn->size; ++i) bytesWritten += printf("   "); //Pad
	                                       bytesWritten += printf("%s %s", insn->mnemonic, insn->op_str); //Write disassembly
	return bytesWritten;
}
