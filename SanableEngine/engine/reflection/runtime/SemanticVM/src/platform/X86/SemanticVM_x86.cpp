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


void SemanticVM::step(const cs_insn* insn, const std::function<void(void*)>&pushCallStack, const std::function<void*()>&popCallStack)
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
	else if (carray_contains(insn->detail->groups, insn->detail->groups_count, cs_group_type::CS_GRP_CALL))
	{
		//Requires target address to be a known const. Will crash otherwise
		SemanticKnownConst& rsp = *canonicalState.registers[X86_REG_RSP]->tryGetKnownConst();
		SemanticValue     & rbp = *canonicalState.registers[X86_REG_RBP];
		SemanticKnownConst& rip = *canonicalState.registers[X86_REG_RIP]->tryGetKnownConst();
		SemanticKnownConst  fp  = *canonicalState.getOperand(insn, 0).tryGetKnownConst();
		
		canonicalState.stackPush(rip); //Push return address to stack
		canonicalState.stackPush(rbp); //Push previous RBP to stack

		//Mark new RBP
		rbp = rsp;

		//Jump to new function
		rip.value = fp.value;

		pushCallStack((uint8_t*)rip.value); //Sanity check. Also no ROP nonsense
	}
	else if (carray_contains(insn->detail->groups, insn->detail->groups_count, cs_group_type::CS_GRP_RET))
	{
		//Cannot pop to an indeterminate address
		if (canonicalState.registers[X86_REG_RBP]->tryGetKnownConst())
		{
			//Pop previous stack frame (return address and RBP) from stack
			SemanticKnownConst& rbp = *canonicalState.registers[X86_REG_RBP]->tryGetKnownConst();
			SemanticKnownConst& rsp = *canonicalState.registers[X86_REG_RSP]->tryGetKnownConst();
			SemanticKnownConst& rip = *canonicalState.registers[X86_REG_RIP]->tryGetKnownConst();
			SemanticValue oldRbp     = canonicalState.stackPop(rbp.size);
			SemanticValue returnAddr = canonicalState.stackPop(rip.size);

			*canonicalState.registers[X86_REG_RIP] = returnAddr; //Jump to return address
			*canonicalState.registers[X86_REG_RBP] = oldRbp; //Pop rbp

			//If given an operand, pop that many bytes from the stack
			if (insn->detail->x86.op_count == 1) canonicalState.stackPop(canonicalState.getOperand(insn, 0).tryGetKnownConst()->value);

			//Sanity check. Also no ROP nonsense
			void* poppedReturnAddr = popCallStack();
			if (poppedReturnAddr) assert(poppedReturnAddr == (void*)returnAddr.tryGetKnownConst()->value);
		}
		else
		{
			//Invalidate state
			*canonicalState.registers[X86_REG_RIP] = SemanticUnknown(sizeof(void*)); //FIXME read proper size
			*canonicalState.registers[X86_REG_RBP] = SemanticUnknown(sizeof(void*));

			//Pop from stack
			canonicalState.stackPop(2 * sizeof(void*)); //FIXME read proper size
				
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
	else if (insn->id == x86_insn::X86_INS_NOP)
	{
		//Do nothing
	}
	else
	{
		//Unhandled operation: Just invalidate all relevant register state
		cs_regs  regsRead,  regsWritten;
		uint8_t nRegsRead, nRegsWritten;
		cs_regs_access(capstone_get_instance(), insn,
			regsRead   , &nRegsRead,
			regsWritten, &nRegsWritten);
		for (int i = 0; i < nRegsWritten; ++i) *(canonicalState.registers[regsWritten[i]]) = SemanticUnknown(sizeof(void*));
	}
}


void SemanticVM::execFunc(void(*fn)(), void(*expectedReturnAddress)(), int indentLevel)
{
	//assert(callConv == CallConv::ThisCall); //That's all we're supporting right now

	//Simulate function, one op at a time
	FunctionBytecodeWalker walker(fn);
	bool endOfFunction;
	do
	{
		endOfFunction = !walker.advance();
		(*canonicalState.registers[X86_REG_RIP]) = SemanticKnownConst((uint_addr_t)walker.codeCursor, sizeof(void*)); //Ensure RIP is up to date

		//DEBUG
		canonicalState.debugPrintWorkingSet();
		printInstructionCursor(walker.insn, indentLevel);

		//Execute current call frame, one opcode at a time
		void(*funcToCall)() = nullptr;
		step(walker.insn,
			[&](void* fn) { funcToCall = (void(*)()) fn; },
			[&]() { return expectedReturnAddress; }
		);

		printf("\n");

		assert(canonicalState.registers[X86_REG_RSP]->tryGetKnownConst() && "Lost track of RSP! This should never happen!");

		//If we're supposed to call another function, do so
		if (funcToCall) execFunc(funcToCall, (void(*)())walker.codeCursor, indentLevel+1);
		
	} while (!endOfFunction);
}
