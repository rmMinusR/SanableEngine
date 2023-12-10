#include "SemanticVM.hpp"

#include "CapstoneWrapper.hpp"
#include "FunctionBytecodeWalker.hpp"

void pad(int& charsPrinted, int desiredWidth)
{
	if (charsPrinted < desiredWidth)
	{
		printf("%*c", desiredWidth-charsPrinted, ' ');
		charsPrinted = desiredWidth;
	}
}

int pad_inline(int charsPrinted, int desiredWidth)
{
	pad(charsPrinted, desiredWidth);
	return charsPrinted;
}


void SemanticVM::step(MachineState& state, const cs_insn* insn, const std::function<void(void*)>& pushCallStack, const std::function<void*()>& popCallStack, const std::function<void(void*)>& jump, const std::function<void(const std::vector<void*>&)>& fork)
{
	if (insn->id == x86_insn::X86_INS_LEA)
	{
		auto addr = state.getOperand(insn, 1);
		state.setOperand(insn, 0, addr);
		printf("   ; = "); addr.debugPrintValue();
	}
	else if (insn->id == x86_insn::X86_INS_MOV)
	{
		state.setOperand(insn, 0, state.getOperand(insn, 1));
	}
	else if (insn_in_group(*insn, cs_group_type::CS_GRP_CALL))
	{
		//Requires target address to be a known const. Will crash otherwise
		SemanticKnownConst rsp = *state.getRegister(X86_REG_RSP).tryGetKnownConst();
		SemanticValue      rbp =  state.getRegister(X86_REG_RBP);
		SemanticKnownConst rip = *state.getRegister(X86_REG_RIP).tryGetKnownConst();
		SemanticKnownConst fp  = *state.getOperand(insn, 0).tryGetKnownConst();
		
		state.stackPush(rip); //Push return address to stack
		state.stackPush(rbp); //Push previous RBP to stack

		//Mark new stack frame location
		rbp = rsp;

		//Jump to new function
		rip = fp;

		//Write back to registers
		state.setRegister(X86_REG_RBP, rbp);
		state.setRegister(X86_REG_RIP, rip);

		pushCallStack((uint8_t*)rip.value); //Sanity check. Also no ROP nonsense
	}
	else if (insn_in_group(*insn, cs_group_type::CS_GRP_RET))
	{
		//Cannot pop to an indeterminate address
		if (state.getRegister(X86_REG_RBP).tryGetKnownConst())
		{
			//Pop previous stack frame (return address and RBP) from stack
			SemanticKnownConst rbp = *state.getRegister(X86_REG_RBP).tryGetKnownConst();
			SemanticKnownConst rip = *state.getRegister(X86_REG_RIP).tryGetKnownConst();
			SemanticValue oldRbp     = state.stackPop(rbp.size);
			SemanticValue returnAddr = state.stackPop(rip.size);

			state.setRegister(X86_REG_RBP, oldRbp); //Restore previous stack frame
			state.setRegister(X86_REG_RIP, returnAddr); //Jump to return address

			//If given an operand, pop that many bytes from the stack
			if (insn->detail->x86.op_count == 1) state.stackPop(state.getOperand(insn, 0).tryGetKnownConst()->value);

			//Sanity check. Also no ROP nonsense
			void* poppedReturnAddr = popCallStack();
			if (poppedReturnAddr) assert(poppedReturnAddr == (void*)returnAddr.tryGetKnownConst()->value);
		}
		else
		{
			//Invalidate state
			size_t ripSize = state.getRegister(X86_REG_RIP).getSize();
			size_t rbpSize = state.getRegister(X86_REG_RBP).getSize();
			state.setRegister(X86_REG_RIP, SemanticUnknown(ripSize) );
			state.setRegister(X86_REG_RBP, SemanticUnknown(rbpSize) );

			//Pop from stack
			state.stackPop(ripSize + rbpSize);
				
			//If given an operand, pop that many bytes from the stack
			if (insn->detail->x86.op_count == 1) state.stackPop(state.getOperand(insn, 0).tryGetKnownConst()->value);

			//Pop from our sanity checker
			popCallStack();
		}
	}
	else if (insn->id == x86_insn::X86_INS_PUSH)
	{
		state.stackPush(state.getOperand(insn, 0));
	}
	else if (insn->id == x86_insn::X86_INS_POP)
	{
		state.setOperand(insn, 0, state.stackPop(insn->detail->x86.operands[0].size));
	}
	else if (insn->id == x86_insn::X86_INS_XCHG)
	{
		auto val1 = state.getOperand(insn, 0);
		auto val2 = state.getOperand(insn, 1);
		state.setOperand(insn, 0, val2);
		state.setOperand(insn, 1, val1);
	}
	else if (insn->id == x86_insn::X86_INS_SUB)
	{
		state.setOperand(insn, 0, state.getOperand(insn, 0)-state.getOperand(insn, 1));
	}
	else if (insn->id == x86_insn::X86_INS_ADD)
	{
		state.setOperand(insn, 0, state.getOperand(insn, 0)+state.getOperand(insn, 1));
	}
	else if (insn->id == x86_insn::X86_INS_DEC)
	{
		auto op = state.getOperand(insn, 0);
		state.setOperand(insn, 0, op-SemanticKnownConst(1, op.getSize()) );
	}
	else if (insn->id == x86_insn::X86_INS_INC)
	{
		auto op = state.getOperand(insn, 0);
		state.setOperand(insn, 0, op+SemanticKnownConst(1, op.getSize()) );
	}
	else if (insn->id == x86_insn::X86_INS_NOP)
	{
		//Do nothing
	}
	else if (insn_in_group(*insn, cs_group_type::CS_GRP_BRANCH_RELATIVE)) //TODO: Really hope nobody uses absolute branching. Is that a thing?
	{
		std::optional<bool> conditionMet = state.isConditionMet(insn->id);

		auto* ifBranch = state.getOperand(insn, 0).tryGetKnownConst();
		auto* noBranch = state.getRegister(x86_reg::X86_REG_RIP).tryGetKnownConst();
		assert(ifBranch && "Indirect branching currently not supported");
		assert(noBranch && "Indirect branching currently not supported");

		//Indeterminate: fork
		if (!conditionMet.has_value())
		{
			assert(ifBranch->value > noBranch->value && "Indeterminate looping currently not supported");
			fork({
				(void*)noBranch->value,
				(void*)ifBranch->value
			});
		}

		//Determinate: jump if condition met
		else
		{
			if (conditionMet.value()) jump(ifBranch);
		}
	}
	else
	{
		//Unhandled operation: Just invalidate all relevant register state
		cs_regs  regsRead,  regsWritten;
		uint8_t nRegsRead, nRegsWritten;
		cs_regs_access(capstone_get_instance(), insn,
			regsRead   , &nRegsRead,
			regsWritten, &nRegsWritten);
		for (int i = 0; i < nRegsWritten; ++i) state.setRegister((x86_reg)regsWritten[i], SemanticUnknown(sizeof(void*)) ); //TODO read correct size
	}
}

void SemanticVM::execFunc_internal(MachineState& state, void(*fn)(), void(*expectedReturnAddress)(), int indentLevel)
{
	//Prepare capstone disassembler
	cs_insn* insn = cs_malloc(capstone_get_instance());

	//Prepare branch list
	struct branch_t
	{
		const uint8_t* cursor;
		MachineState state;
		bool keepExecuting;
	};
	std::vector<branch_t> branches = {
		{ (uint8_t*)fn, state, true }
	};

	while (true)
	{
		//Canonize branches with the same cursor
		for (int i = 0; i < branches.size(); ++i)
		{
			for (int j = i+1; j < branches.size(); ++j)
			{
				if (branches[i].cursor == branches[j].cursor && branches[i].keepExecuting && branches[j].keepExecuting)
				{
					branches[i].state = MachineState::merge({ &branches[i].state, &branches[j].state }); //Canonize state
					branches.erase(branches.begin()+i); //Erase copy
					j--; //Don't skip!
				}
			}
		}

		//Execute the cursor that's furthest behind
		branch_t* toExec = nullptr;
		for (int i = 0; i < branches.size(); ++i)
		{
			if (branches[i].keepExecuting && (toExec == nullptr || branches[i].cursor < toExec->cursor))
			{
				toExec = &branches[i];
			}
		}
		if (toExec == nullptr) break; //All branches have terminated

		//Advance cursor and interpret next
		uint64_t addr = (uint64_t)(uint_addr_t)(toExec->cursor);
		size_t allowedToProcess = sizeof(cs_insn::bytes); //No way to know for sure, but we can do some stuff with JUMP/RET detection to figure it out
		bool disassemblyGood = cs_disasm_iter(capstone_get_instance(), &toExec->cursor, &allowedToProcess, &addr, insn);
		assert(disassemblyGood && "An internal error occurred with the Capstone disassembler.");
		
		//Update emulated instruction pointer
		SemanticValue rip = toExec->state.getRegister(X86_REG_RIP);
		toExec->state.setRegister(X86_REG_RIP, SemanticKnownConst(addr, rip.getSize()) ); //Ensure RIP is up to date

		//DEBUG
		toExec->state.debugPrintWorkingSet();
		printInstructionCursor(insn, indentLevel);

		//Execute current call frame, one opcode at a time
		void* callTarget = nullptr;
		std::vector<void*> jmpTargets; //Empty if no JMP, 1 element if determinate JMP, 2+ elements if indeterminate JMP
		step(toExec->state, insn,
			[&](void* fn) { callTarget = fn; }, //On CALL
			[&]() { toExec->keepExecuting = false; return expectedReturnAddress; }, //On RET
			[&](void* jmp) { jmpTargets = { jmp }; }, //On jump
			[&](const std::vector<void*>& forks) { jmpTargets = forks; } //On fork
		);
		
		//DEBUG
		printf("\n");

		//If we're supposed to call another function, do so
		if (callTarget) execFunc_internal(toExec->state, (void(*)())callTarget, (void(*)())toExec->cursor, indentLevel+1);

		//Handle jumping/branching
		if (jmpTargets.size() == 1) toExec->cursor = (uint8_t*)jmpTargets[0]; //Determinate case: jump
		else if (jmpTargets.size() > 1) //Indeterminate case: branch
		{
			for (void* i : jmpTargets) assert(i >= toExec->cursor && "Indeterminate looping not supported");

			toExec->cursor = (uint8_t*)jmpTargets[0];
			for (int i = 1; i < jmpTargets.size(); ++i)
			{
				branches.push_back({ (uint8_t*)jmpTargets[i], toExec->state, true });
			}
		}

		assert(toExec->state.getRegister(X86_REG_RSP).tryGetKnownConst() && "Lost track of RSP! This should never happen!");
	}

	//Canonize all remaining states
	std::vector<const MachineState*> divergentStates;
	for (const branch_t& branch : branches) divergentStates.push_back(&branch.state);
	state = MachineState::merge(divergentStates);

	//Cleanup capstone disassembler
	cs_free(insn, 1);
}

void SemanticVM::execFunc(MachineState& state, void(*fn)())
{
	//assert(callConv == CallConv::ThisCall); //That's all we're supporting right now

	//Setup: set flags
	state.setRegister(X86_REG_RIP, SemanticUnknown(sizeof(void*)) ); //TODO: 32-bit-on-64 support?
	state.setRegister(X86_REG_RBP, SemanticUnknown(sizeof(void*)) ); //Caller is indeterminate. TODO: 32-bit-on-64 support?
	state.setRegister(X86_REG_RSP, SemanticKnownConst(-2 * sizeof(void*), sizeof(void*)) ); //TODO: This is a magic value, the size of one stack frame. Should be treated similarly to ThisPtr instead.
	{
		SemanticFlags flags;
		flags.bits = 0;
		flags.bitsKnown = ~0ull;
		state.setRegister(X86_REG_EFLAGS, flags);
	}

	//Invoke
	execFunc_internal(state, fn, nullptr, 0);
	state.debugPrintWorkingSet();
	printf("\n");

	//Ensure output parity
	SemanticValue rsp = state.getRegister(X86_REG_RSP);
	SemanticKnownConst* const_rsp = rsp.tryGetKnownConst();
	assert(const_rsp && const_rsp->value == 0); //TODO handle return value
}
