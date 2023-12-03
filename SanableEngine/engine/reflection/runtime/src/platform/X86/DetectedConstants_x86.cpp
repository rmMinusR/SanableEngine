#include "DetectedConstants.hpp"

#include "CapstoneWrapper.hpp"

#include "FunctionBytecodeWalker.hpp"
#include "MachineState.hpp"

DetectedConstants DetectedConstants::captureCtor(size_t objSize, void(*ctor)())
{
	//Setup VM, __thiscall format
	MachineState state;
	(*state.registers[X86_REG_RAX]) = SemanticThisPtr();

	//Simulate function, one op at a time
	FunctionBytecodeWalker walker(ctor);
	while (walker.advance())
	{
		if (walker.insn->id == x86_insn::X86_INS_MOV)
		{
			state.setOperand(walker.insn, 0, state.getOperand(walker.insn, 1));
		}
		else if (walker.insn->id == x86_insn::X86_INS_XCHG)
		{
			auto val2 = state.getOperand(walker.insn, 0);
			auto val1 = state.getOperand(walker.insn, 1);
			state.setOperand(walker.insn, 0, val2);
			state.setOperand(walker.insn, 1, val1);
		}
		else if (walker.insn->id == x86_insn::X86_INS_NOP)
		{
			//Do nothing
		}
		else
		{
			//Unhandled operation: Just invalidate all relevant register state
			cs_regs regsRead   ; uint8_t nRegsRead;
			cs_regs regsWritten; uint8_t nRegsWritten;
			cs_regs_access(capstone_get_instance(), walker.insn,
				regsRead   , &nRegsRead,
				regsWritten, &nRegsWritten);
			for (int i = 0; i < nRegsWritten; ++i) *(state.registers[regsWritten[i]]) = SemanticUnknown();
		}
	}

	//Read from state.thisMemory into DetectedConstants
	DetectedConstants out(objSize);
	for (size_t i = 0; i < objSize; ++i)
	{
		GeneralValue byte = state.getMemory(SemanticThisPtr{ i }, 1);
		if (std::holds_alternative<SemanticKnownConst>(byte))
		{
			out.usage[i] = true;
			out.bytes[i] = std::get<SemanticKnownConst>(byte).value;
		}
	}
	return out;
}
