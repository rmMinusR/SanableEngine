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


void SemanticVM::step(const cs_insn* insn, const std::function<void(void*)>& pushCallStack, const std::function<void*()>& popCallStack, const std::function<void(void*)>& jump, const std::function<void(const std::vector<void*>&)>& fork)
{
	if (insn->id == x86_insn::X86_INS_LEA)
	{
		auto addr = canonicalState.getOperand(insn, 1);
		canonicalState.setOperand(insn, 0, addr);
		printf("   ; = "); addr.debugPrintValue();
	}
	else if (insn->id == x86_insn::X86_INS_MOV)
	{
		canonicalState.setOperand(insn, 0, canonicalState.getOperand(insn, 1));
	}
	else if (insn_in_group(*insn, cs_group_type::CS_GRP_CALL))
	{
		//Requires target address to be a known const. Will crash otherwise
		SemanticKnownConst rsp = *canonicalState.getRegister(X86_REG_RSP).tryGetKnownConst();
		SemanticValue      rbp =  canonicalState.getRegister(X86_REG_RBP);
		SemanticKnownConst rip = *canonicalState.getRegister(X86_REG_RIP).tryGetKnownConst();
		SemanticKnownConst fp  = *canonicalState.getOperand(insn, 0).tryGetKnownConst();
		
		canonicalState.stackPush(rip); //Push return address to stack
		canonicalState.stackPush(rbp); //Push previous RBP to stack

		//Mark new stack frame location
		rbp = rsp;

		//Jump to new function
		rip = fp;

		//Write back to registers
		canonicalState.setRegister(X86_REG_RBP, rbp);
		canonicalState.setRegister(X86_REG_RIP, rip);

		pushCallStack((uint8_t*)rip.value); //Sanity check. Also no ROP nonsense
	}
	else if (insn_in_group(*insn, cs_group_type::CS_GRP_RET))
	{
		//Cannot pop to an indeterminate address
		if (canonicalState.getRegister(X86_REG_RBP).tryGetKnownConst())
		{
			//Pop previous stack frame (return address and RBP) from stack
			SemanticKnownConst rbp = *canonicalState.getRegister(X86_REG_RBP).tryGetKnownConst();
			SemanticKnownConst rip = *canonicalState.getRegister(X86_REG_RIP).tryGetKnownConst();
			SemanticValue oldRbp     = canonicalState.stackPop(rbp.size);
			SemanticValue returnAddr = canonicalState.stackPop(rip.size);

			canonicalState.setRegister(X86_REG_RBP, oldRbp); //Restore previous stack frame
			canonicalState.setRegister(X86_REG_RIP, returnAddr); //Jump to return address

			//If given an operand, pop that many bytes from the stack
			if (insn->detail->x86.op_count == 1) canonicalState.stackPop(canonicalState.getOperand(insn, 0).tryGetKnownConst()->value);

			//Sanity check. Also no ROP nonsense
			void* poppedReturnAddr = popCallStack();
			if (poppedReturnAddr) assert(poppedReturnAddr == (void*)returnAddr.tryGetKnownConst()->value);
		}
		else
		{
			//Invalidate state
			size_t ripSize = canonicalState.getRegister(X86_REG_RIP).getSize();
			size_t rbpSize = canonicalState.getRegister(X86_REG_RBP).getSize();
			canonicalState.setRegister(X86_REG_RIP, SemanticUnknown(ripSize) );
			canonicalState.setRegister(X86_REG_RBP, SemanticUnknown(rbpSize) );

			//Pop from stack
			canonicalState.stackPop(ripSize + rbpSize);
				
			//If given an operand, pop that many bytes from the stack
			if (insn->detail->x86.op_count == 1) canonicalState.stackPop(canonicalState.getOperand(insn, 0).tryGetKnownConst()->value);

			//Pop from our sanity checker
			popCallStack();
		}
	}
	else if (insn->id == x86_insn::X86_INS_PUSH)
	{
		canonicalState.stackPush(canonicalState.getOperand(insn, 0));
	}
	else if (insn->id == x86_insn::X86_INS_POP)
	{
		canonicalState.setOperand(insn, 0, canonicalState.stackPop(insn->detail->x86.operands[0].size));
	}
	else if (insn->id == x86_insn::X86_INS_XCHG)
	{
		auto val1 = canonicalState.getOperand(insn, 0);
		auto val2 = canonicalState.getOperand(insn, 1);
		canonicalState.setOperand(insn, 0, val2);
		canonicalState.setOperand(insn, 1, val1);
	}
	else if (insn->id == x86_insn::X86_INS_SUB)
	{
		canonicalState.setOperand(insn, 0, canonicalState.getOperand(insn, 0)-canonicalState.getOperand(insn, 1));
	}
	else if (insn->id == x86_insn::X86_INS_ADD)
	{
		canonicalState.setOperand(insn, 0, canonicalState.getOperand(insn, 0)+canonicalState.getOperand(insn, 1));
	}
	else if (insn->id == x86_insn::X86_INS_DEC)
	{
		auto op = canonicalState.getOperand(insn, 0);
		canonicalState.setOperand(insn, 0, op-SemanticKnownConst(1, op.getSize()) );
	}
	else if (insn->id == x86_insn::X86_INS_INC)
	{
		auto op = canonicalState.getOperand(insn, 0);
		canonicalState.setOperand(insn, 0, op+SemanticKnownConst(1, op.getSize()) );
	}
	else if (insn->id == x86_insn::X86_INS_NOP)
	{
		//Do nothing
	}
	else if (insn_in_group(*insn, cs_group_type::CS_GRP_BRANCH_RELATIVE)) //TODO: Really hope nobody uses absolute branching. Is that a thing?
	{
		std::optional<bool> conditionMet = canonicalState.isConditionMet(insn->id);

		auto* ifBranch = canonicalState.getOperand(insn, 0).tryGetKnownConst();
		auto* noBranch = canonicalState.getRegister(x86_reg::X86_REG_RIP).tryGetKnownConst();
		assert(ifBranch && "Indirect branching currently not supported");
		assert(noBranch && "Indirect branching currently not supported");

		//Indeterminate: fork
		if (!conditionMet.has_value())
		{
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
		for (int i = 0; i < nRegsWritten; ++i) canonicalState.setRegister((x86_reg)regsWritten[i], SemanticUnknown(sizeof(void*)) ); //TODO read correct size
	}
}

void SemanticVM::execFunc_internal(void(*fn)(), void(*expectedReturnAddress)(), int indentLevel)
{
	//assert(callConv == CallConv::ThisCall); //That's all we're supporting right now
	
	//Simulate function, one op at a time
	FunctionBytecodeWalker walker(fn);
	bool endOfFunction;
	do
	{
		endOfFunction = !walker.advance();
		size_t ripSize = canonicalState.getRegister(X86_REG_RIP).getSize();
		canonicalState.setRegister(X86_REG_RIP, SemanticKnownConst((uint_addr_t)walker.codeCursor, ripSize) ); //Ensure RIP is up to date

		//DEBUG
		canonicalState.debugPrintWorkingSet();
		printInstructionCursor(walker.insn, indentLevel);

		//Execute current call frame, one opcode at a time
		void(*funcToCall)() = nullptr;
		std::vector<void*> jmpTargets; //Empty if no JMP, 1 element if determinate JMP, 2+ elements if indeterminate JMP
		step(walker.insn,
			[&](void* fn) { funcToCall = (void(*)()) fn; }, //On CALL
			[&]() { return expectedReturnAddress; }, //On RET
			[&](void* jmp) { jmpTargets = { jmp }; }, //On jump
			[&](const std::vector<void*>& forks) { jmpTargets = forks; } //On fork
		);

		printf("\n");

		assert(canonicalState.getRegister(X86_REG_RSP).tryGetKnownConst() && "Lost track of RSP! This should never happen!");

		//If we're supposed to call another function, do so
		if (funcToCall) execFunc_internal(funcToCall, (void(*)())walker.codeCursor, indentLevel+1);

		//Handle forking/jumping
		if (jmpTargets.size() == 1) walker.codeCursor = (uint8_t*)jmpTargets[0];
		else if (jmpTargets.size() > 1) execBranch(canonicalState, jmpTargets);
		
	} while (!endOfFunction);
}

void SemanticVM::execFunc(void(*fn)())
{
	//Setup
	canonicalState.setRegister(X86_REG_RIP, SemanticUnknown(sizeof(void*)) ); //TODO: 32-bit-on-64 support?
	canonicalState.setRegister(X86_REG_RBP, SemanticUnknown(sizeof(void*)) ); //Caller is indeterminate. TODO: 32-bit-on-64 support?
	canonicalState.setRegister(X86_REG_RSP, SemanticKnownConst(-2 * sizeof(void*), sizeof(void*)) ); //TODO: This is a magic value, the size of one stack frame. Should be treated similarly to ThisPtr instead.

	//Invoke
	execFunc_internal(fn, nullptr, 0);
	canonicalState.debugPrintWorkingSet();
	printf("\n");

	//Ensure output parity
	SemanticValue rsp = canonicalState.getRegister(X86_REG_RSP);
	SemanticKnownConst* const_rsp = rsp.tryGetKnownConst();
	assert(const_rsp && const_rsp->value == 0); //TODO handle return value
}
