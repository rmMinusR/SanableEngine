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

#pragma region Debugging prints

int debugPrintSignedHex(int64_t val)
{
	if (val >= 0) return printf("+0x%llx", val);
	else          return printf("-0x%llx", 1+(~uint64_t(0)-uint64_t(val)));
}

int debugPrintValue(GeneralValue val)
{
	int nWritten = 0;

	     if (std::holds_alternative<SemanticKnownConst>(val)) nWritten = debugPrintSignedHex(std::get<SemanticKnownConst>(val).value);
	else if (std::holds_alternative<SemanticThisPtr>(val)) nWritten = printf("this") + debugPrintSignedHex(std::get<SemanticThisPtr>(val).offset);
	else if (std::holds_alternative<SemanticUnknown>(val)) nWritten = printf("(unknown)");
	else assert(false);

	int nToPad = 9-nWritten;
	if (nToPad > 0) printf("%*c", nToPad, ' ');

	return nWritten + std::max(0, nToPad);
}

int printMachineState(const MachineState& state)
{
	int bytesWritten = 0;
	bytesWritten += printf("rcx=");
	bytesWritten += debugPrintValue(*state.registers[X86_REG_RCX]);
	bytesWritten += printf(" | rax=");
	bytesWritten += debugPrintValue(*state.registers[X86_REG_RAX]);
	bytesWritten += printf(" | rsp=");
	bytesWritten += debugPrintValue(*state.registers[X86_REG_RSP]);
	//bytesWritten += printf(" | rbp=");
	//bytesWritten += debugPrint(*state.registers[X86_REG_RBP]);
	return bytesWritten;
}

int printInstructionCursor(const cs_insn* insn, int callLevel)
{
	int bytesWritten = 0;
	for (int i = 0; i < callLevel   ; ++i) bytesWritten += printf(" |  "); //Indent
	//                                     bytesWritten += printf("%p: ", (void*)walker.insn->address); //Write address
	for (int i = 0; i < insn->size  ; ++i) bytesWritten += printf(" %02x", insn->bytes[i]); //Write raw bytes
	for (int i = 0; i < 8-insn->size; ++i) bytesWritten += printf("   "); //Pad
	                                       bytesWritten += printf("    %s %s", insn->mnemonic, insn->op_str); //Write disassembly
	return bytesWritten;
}

int printLEA(const MachineState& state, const cs_insn* insn)
{
	assert(insn->id == x86_insn::X86_INS_LEA);

	int bytesWritten = 0;

	auto dst = state.getOperand(insn, 0);
	auto value = state.getOperand(insn, 1);
	bytesWritten += printf("*("); bytesWritten += debugPrintValue(dst);
	bytesWritten += printf(") := "); bytesWritten += debugPrintValue(value);

	return bytesWritten;
}

void pad(int& charsPrinted, int desiredWidth)
{
	assert(charsPrinted < desiredWidth);
	printf("%*c", desiredWidth-charsPrinted, ' ');
	charsPrinted = desiredWidth;
}

#pragma endregion

void tickVM(MachineState& state, const cs_insn* insn)
{

}

DetectedConstants DetectedConstants::captureCtor(size_t objSize, void(*ctor)())
{
	//Setup VM
	MachineState state;
	(*state.registers[X86_REG_RBP]) = SemanticUnknown(); //Caller is indeterminate
	(*state.registers[X86_REG_RSP]) = SemanticKnownConst(-2*sizeof(void*), sizeof(void*)); //TODO: This is a magic value, the size of one stack frame. Should be treated similarly to ThisPtr instead.
	(*state.registers[X86_REG_ECX]) = SemanticThisPtr(0); //__thiscall: caller puts address of class object in eCX (see <https://en.wikibooks.org/wiki/X86_Disassembly/Calling_Conventions#THISCALL>)
	
	//Simulate function, one op at a time
	std::vector<FunctionBytecodeWalker> callstack;
	callstack.emplace_back(ctor);
	while (callstack.size() > 0)
	{
		//FunctionBytecodeWalker& walker = callstack[callstack.size() - 1];
		#define walker callstack[callstack.size()-1]
		bool endOfFunction = !walker.advance();
		(*state.registers[X86_REG_RIP]) = SemanticKnownConst((uint_addr_t)walker.codeCursor, sizeof(void*)); //Ensure RIP is up to date

		{
			int charsPrinted = 0;
			charsPrinted += printInstructionCursor(walker.insn, callstack.size() - 1);
			pad(charsPrinted, 70);
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
			//Requires target address to be a known const. Will crash otherwise
			assert(std::holds_alternative<SemanticKnownConst>(state.getOperand(walker.insn, 0)));

			SemanticKnownConst& rsp = std::get<SemanticKnownConst>(*state.registers[X86_REG_RSP]);
			GeneralValue      & rbp = *state.registers[X86_REG_RBP];
			SemanticKnownConst& rip = std::get<SemanticKnownConst>(*state.registers[X86_REG_RIP]);
			SemanticKnownConst fp = std::get<SemanticKnownConst>(state.getOperand(walker.insn, 0));

			//Push return address to stack
			rsp.value -= rip.size; state.setMemory((void*)rsp.value, rip, rip.size);

			//Push previous RBP to stack
			rsp.value -= sizeof(void*); state.setMemory((void*)rsp.value, rbp, sizeof(void*));

			//Mark new RBP
			rbp = rsp;

			//Jump to new function
			rip.value = fp.value;

			callstack.emplace_back((uint8_t*)rip.value); //Sanity check. Also no ROP nonsense
		}
		else if (carray_contains(walker.insn->detail->groups, walker.insn->detail->groups_count, cs_group_type::CS_GRP_RET))
		{
			if (callstack.size() > 1)
			{
				//Cannot pop to an indeterminate address.
				assert(std::holds_alternative<SemanticKnownConst>(*state.registers[X86_REG_RBP]));

				//Pop previous stack frame (return address and RBP) from stack
				SemanticKnownConst& rbp = std::get<SemanticKnownConst>(*state.registers[X86_REG_RBP]);
				SemanticKnownConst& rsp = std::get<SemanticKnownConst>(*state.registers[X86_REG_RSP]);
				GeneralValue oldRbp     = state.getMemory((void*)(rbp.value              ), sizeof(void*));
				GeneralValue returnAddr = state.getMemory((void*)(rbp.value+sizeof(void*)), sizeof(void*));
				rsp.value += 2*sizeof(void*);

				*state.registers[X86_REG_RIP] = returnAddr; //Jump to return address
				*state.registers[X86_REG_RBP] = oldRbp; //Pop rbp

				//If given an operand, pop that many bytes from the stack
				if (walker.insn->detail->x86.op_count == 1)
				{
					size_t opSize = std::get<SemanticKnownConst>(state.getOperand(walker.insn, 0)).value;
					SemanticKnownConst& rsp = std::get<SemanticKnownConst>(*state.registers[X86_REG_RSP]);
					state.setOperand(walker.insn, 0, state.getMemory((void*)rsp.value, opSize)); //Read from stack
					rsp.value += opSize; //Reclaim space on stack
				}

				//Sanity check. Also no ROP nonsense
				callstack.pop_back();
				assert(callstack[callstack.size()-1].codeCursor == (void*)std::get<SemanticKnownConst>(returnAddr).value);
			}
			else
			{
				//Invalidate state
				*state.registers[X86_REG_RIP] = SemanticUnknown();
				*state.registers[X86_REG_RBP] = SemanticUnknown();

				//Pop from stack
				std::get<SemanticKnownConst>(*state.registers[X86_REG_RSP]).value += 2*sizeof(void*);
				
				//If given an operand, pop that many bytes from the stack
				if (walker.insn->detail->x86.op_count == 1)
				{
					size_t opSize = std::get<SemanticKnownConst>(state.getOperand(walker.insn, 0)).value;
					SemanticKnownConst& rsp = std::get<SemanticKnownConst>(*state.registers[X86_REG_RSP]);
					state.setOperand(walker.insn, 0, state.getMemory((void*)rsp.value, opSize)); //Read from stack
					rsp.value += opSize; //Reclaim space on stack
				}

				//Pop from our sanity checker
				callstack.pop_back();
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

		{
			int charsPrinted = 0;
			charsPrinted += printf(" < ");
			charsPrinted += printMachineState(state);
			pad(charsPrinted, 50);
			if (callstack.size() > 0 && walker.insn->id == x86_insn::X86_INS_LEA) charsPrinted += printf(" ;   ") + printLEA(state, walker.insn);
			charsPrinted += printf("\n");
		}

		if (!std::holds_alternative<SemanticKnownConst>(*state.registers[X86_REG_RSP]))
		{
			printf("WARNING: Lost track of RSP! This should never happen!\n");
		}
	}

	assert(std::holds_alternative<SemanticKnownConst>(*state.registers[X86_REG_RSP])
		&& std::get<SemanticKnownConst>(*state.registers[X86_REG_RSP]).value == 0);

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
