#include "DetectedConstants.hpp"

#include "CapstoneWrapper.hpp"

#include "FunctionBytecodeWalker.hpp"
#include "MachineState.hpp"

template<typename T1, typename T2, typename T3, typename T4>
GeneralValue doMathOp(GeneralValue arg1, GeneralValue arg2,
	T1 funcConstConst, T2 funcConstThis, T3 funcThisConst, T4 funcThisThis)
{
		 if (std::holds_alternative<SemanticUnknown   >(arg1) || std::holds_alternative<SemanticUnknown   >(arg2)) return SemanticUnknown();
	else if (std::holds_alternative<SemanticKnownConst>(arg1) && std::holds_alternative<SemanticKnownConst>(arg2)) return funcConstConst(std::get<SemanticKnownConst>(arg1), std::get<SemanticKnownConst>(arg2));
	else if (std::holds_alternative<SemanticKnownConst>(arg1) && std::holds_alternative<SemanticThisPtr   >(arg2)) return funcConstThis (std::get<SemanticKnownConst>(arg1), std::get<SemanticThisPtr   >(arg2));
	else if (std::holds_alternative<SemanticThisPtr   >(arg1) && std::holds_alternative<SemanticKnownConst>(arg2)) return funcThisConst (std::get<SemanticThisPtr   >(arg1), std::get<SemanticKnownConst>(arg2));
	else if (std::holds_alternative<SemanticThisPtr   >(arg1) && std::holds_alternative<SemanticThisPtr   >(arg2)) return funcThisThis  (std::get<SemanticThisPtr   >(arg1), std::get<SemanticThisPtr   >(arg2));
	else
	{
		assert(false);
		return GeneralValue();
	}
}

DetectedConstants DetectedConstants::captureCtor(size_t objSize, void(*ctor)())
{
	//Setup VM
	MachineState state;
	(*state.registers[X86_REG_RSP]) = SemanticKnownConst(~uint64_t(0), sizeof(void*)); //TODO: This is also a magic value, and should not be treated as a known quantity
	(*state.registers[X86_REG_ECX]) = SemanticThisPtr(); //__thiscall: caller puts address of class object in eCX (see <https://en.wikibooks.org/wiki/X86_Disassembly/Calling_Conventions#THISCALL>)
	
	//Simulate function, one op at a time
	FunctionBytecodeWalker walker(ctor);
	while (walker.advance())
	{
		if (walker.insn->id == x86_insn::X86_INS_LEA)
		{
			state.setMemory(state.getOperand(walker.insn, 0), state.getOperand(walker.insn, 1), sizeof(void*));
		}
		else if (walker.insn->id == x86_insn::X86_INS_MOV)
		{
			state.setOperand(walker.insn, 0, state.getOperand(walker.insn, 1));
		}
		else if (walker.insn->id == x86_insn::X86_INS_XCHG)
		{
			auto val1 = state.getOperand(walker.insn, 0);
			auto val2 = state.getOperand(walker.insn, 1);
			state.setOperand(walker.insn, 0, val2);
			state.setOperand(walker.insn, 1, val1);
		}
		else if (walker.insn->id == x86_insn::X86_INS_SUB)
		{
			state.setOperand(walker.insn, 0, doMathOp(state.getOperand(walker.insn, 0), state.getOperand(walker.insn, 1),
				[](const SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.value-arg2.value, std::min(arg1.size, arg2.size)); },
				[](const SemanticKnownConst arg1, SemanticThisPtr    arg2) { return SemanticUnknown(); },
				[](const SemanticThisPtr    arg1, SemanticKnownConst arg2) { return SemanticThisPtr{ arg1.offset - arg2.value }; },
				[](const SemanticThisPtr    arg1, SemanticThisPtr    arg2) { return SemanticKnownConst(arg1.offset-arg2.offset, sizeof(void*)); }
			));
		}
		else if (walker.insn->id == x86_insn::X86_INS_ADD)
		{
			state.setOperand(walker.insn, 0, doMathOp(state.getOperand(walker.insn, 0), state.getOperand(walker.insn, 1),
				[](const SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.value+arg2.value, std::min(arg1.size, arg2.size)); },
				[](const SemanticKnownConst arg1, SemanticThisPtr    arg2) { return SemanticThisPtr{ arg1.value + arg2.offset }; },
				[](const SemanticThisPtr    arg1, SemanticKnownConst arg2) { return SemanticThisPtr{ arg1.offset + arg2.value }; },
				[](const SemanticThisPtr    arg1, SemanticThisPtr    arg2) { return SemanticUnknown(); }
			));
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
