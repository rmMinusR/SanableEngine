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

int debugPrintSignedHex(int64_t val)
{
	if (val >= 0) return printf("+0x%llx", val);
	else          return printf("-0x%llx", 1+(~uint64_t(0)-uint64_t(val)));
}

void debugPrint(GeneralValue val)
{
	int nWritten = 0;
	     if (std::holds_alternative<SemanticKnownConst>(val)) nWritten = debugPrintSignedHex(std::get<SemanticKnownConst>(val).value);
	else if (std::holds_alternative<SemanticThisPtr>(val)) nWritten = printf("this") + debugPrintSignedHex(std::get<SemanticThisPtr>(val).offset);
	else if (std::holds_alternative<SemanticUnknown>(val)) nWritten = printf("(unknown)");
	else assert(false);

	int nToPad = 9-nWritten;
	if (nToPad > 0) printf("%*c", nToPad, ' ');
}

DetectedConstants DetectedConstants::captureCtor(size_t objSize, void(*ctor)())
{
	//Setup VM
	MachineState state;
	(*state.registers[X86_REG_RBP]) = (*state.registers[X86_REG_RSP]) = SemanticKnownConst(0, sizeof(void*)); //TODO: This is also a magic value, and should not be treated as a known quantity
	(*state.registers[X86_REG_ECX]) = SemanticThisPtr(0); //__thiscall: caller puts address of class object in eCX (see <https://en.wikibooks.org/wiki/X86_Disassembly/Calling_Conventions#THISCALL>)
	
	//Simulate function, one op at a time
	std::vector<FunctionBytecodeWalker> callstack;
	callstack.emplace_back(ctor);
	while (callstack.size() > 0)
	{
		FunctionBytecodeWalker& walker = callstack[callstack.size() - 1];
		bool endOfFunction = !walker.advance();
		(*state.registers[X86_REG_RIP]) = SemanticKnownConst((uint_addr_t)walker.codeCursor, sizeof(void*)); //Ensure RIP is up to date
		
		//DEBUG
		{
			printf(" rcx=");
			debugPrint(*state.registers[X86_REG_RCX]);
			printf(" | rax=");
			debugPrint(*state.registers[X86_REG_RAX]);
			printf(" | rsp=");
			debugPrint(*state.registers[X86_REG_RSP]);
			//printf(" | rbp=");
			//debugPrint(*state.registers[X86_REG_RBP]);
			printf(" |> ");

			for (int i = 0; i < callstack.size()-1; ++i) printf(" |  "); //Indent
			//printf("%p: ", (void*)walker.insn->address); //Write address
			for (int i = 0; i < walker.insn->size; ++i) printf(" %02x", walker.insn->bytes[i]); //Write raw bytes
			for (int i = 0; i < 8-walker.insn->size; ++i) printf("   "); //Pad
			printf("    %s %s", walker.insn->mnemonic, walker.insn->op_str); //Write disassembly

			if (walker.insn->id == x86_insn::X86_INS_LEA)
			{
				auto dst = state.getOperand(walker.insn, 0);
				auto value = state.getOperand(walker.insn, 1);
				printf(" ;   *("); debugPrint(dst);
				printf(") := "); debugPrint(value);
			}

			printf("\n");
		}

		//Execute current call frame, one opcode at a time
		
		if (walker.insn->id == x86_insn::X86_INS_LEA)
		{
			auto dst = state.getOperand(walker.insn, 0);
			auto addr = state.getOperand(walker.insn, 1);
			state.setMemory(dst, addr, sizeof(void*));
		}
		else if (walker.insn->id == x86_insn::X86_INS_MOV)
		{
			state.setOperand(walker.insn, 0, state.getOperand(walker.insn, 1));
		}
		else if (carray_contains(walker.insn->detail->groups, walker.insn->detail->groups_count, cs_group_type::CS_GRP_CALL))
		{
			auto fp = state.getOperand(walker.insn, 0);
			if (std::holds_alternative<SemanticKnownConst>(fp)) callstack.emplace_back( (void(*)()) std::get<SemanticKnownConst>(fp).value );
			else
			{
				printf("WARNING: Could not deduce CALL location\n");
				printf("\t@%p:", (void*)walker.insn->address);
				for (size_t i = 0; i < walker.insn->size; ++i) printf(" %2x", (uint32_t)walker.insn->bytes[i]);
				printf("   ; %s %s\n", walker.insn->mnemonic, walker.insn->op_str);
			}
		}
		else if (walker.insn->id == x86_insn::X86_INS_PUSH)
		{
			size_t opSize = walker.insn->detail->x86.operands[0].size;
			SemanticKnownConst& rsp = std::get<SemanticKnownConst>(*state.registers[X86_REG_RSP]);
			rsp.value -= opSize; //Make space on stack
			state.setMemory((void*)rsp.value, state.getOperand(walker.insn, 0), opSize); //Write to stack
		}
		else if (walker.insn->id == x86_insn::X86_INS_POP)
		{
			size_t opSize = walker.insn->detail->x86.operands[0].size;
			SemanticKnownConst& rsp = std::get<SemanticKnownConst>(*state.registers[X86_REG_RSP]);
			state.setOperand(walker.insn, 0, state.getMemory((void*)rsp.value, opSize)); //Read from stack
			rsp.value += opSize; //Reclaim space on stack
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
				[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.value-arg2.value, std::min(arg1.size, arg2.size)); },
				[](SemanticKnownConst arg1, SemanticThisPtr    arg2) { return SemanticUnknown(); },
				[](SemanticThisPtr    arg1, SemanticKnownConst arg2) { return SemanticThisPtr{ arg1.offset - arg2.value }; },
				[](SemanticThisPtr    arg1, SemanticThisPtr    arg2) { return SemanticKnownConst(arg1.offset - arg2.offset, sizeof(void*)); }
			));
		}
		else if (walker.insn->id == x86_insn::X86_INS_ADD)
		{
			state.setOperand(walker.insn, 0, doMathOp(state.getOperand(walker.insn, 0), state.getOperand(walker.insn, 1),
				[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.value+arg2.value, std::min(arg1.size, arg2.size)); },
				[](SemanticKnownConst arg1, SemanticThisPtr    arg2) { return SemanticThisPtr{ arg1.value + arg2.offset }; },
				[](SemanticThisPtr    arg1, SemanticKnownConst arg2) { return SemanticThisPtr{ arg1.offset + arg2.value }; },
				[](SemanticThisPtr    arg1, SemanticThisPtr    arg2) { return SemanticUnknown(); }
			));
		}
		else if (walker.insn->id == x86_insn::X86_INS_NOP)
		{
			//Do nothing
		}
		else if (carray_contains(walker.insn->detail->groups, walker.insn->detail->groups_count, cs_group_type::CS_GRP_RET))
		{
			if (walker.insn->detail->x86.op_count != 0)
			{
				size_t opSize = std::get<SemanticKnownConst>(state.getOperand(walker.insn, 0)).value;
				SemanticKnownConst& rsp = std::get<SemanticKnownConst>(*state.registers[X86_REG_RSP]);
				state.setOperand(walker.insn, 0, state.getMemory((void*)rsp.value, opSize)); //Read from stack
				rsp.value += opSize; //Reclaim space on stack
			}
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

		if (!std::holds_alternative<SemanticKnownConst>(*state.registers[X86_REG_RSP]))
		{
			printf("WARNING: Lost track of RSP! This should never happen!\n");
		}

		if (endOfFunction)
		{
			//We returned, remove current call frame
			callstack.pop_back();
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
