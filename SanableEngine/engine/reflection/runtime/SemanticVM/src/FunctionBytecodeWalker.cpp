#include "FunctionBytecodeWalker.hpp"

#include <cassert>

#include "CapstoneWrapper.hpp"

FunctionBytecodeWalker::FunctionBytecodeWalker(const uint8_t* data)
{
	codeCursor = data;
	insn = cs_malloc(capstone_get_instance());
	furthestKnownJump = nullptr;
}

FunctionBytecodeWalker::FunctionBytecodeWalker(void(*fn)()) :
	FunctionBytecodeWalker(reinterpret_cast<uint8_t*>(fn))
{
}

FunctionBytecodeWalker::~FunctionBytecodeWalker()
{
	if (insn)
	{
		cs_free(insn, 1);
		insn = nullptr;
	}
}

bool FunctionBytecodeWalker::advance()
{
	//Advance cursor and interpret next
	uint64_t addr = (uint64_t)reinterpret_cast<uint_addr_t>(codeCursor);
	size_t allowedToProcess = sizeof(cs_insn::bytes); //No way to know for sure, but we can do some stuff with JUMP/RET detection to figure it out
	bool status = cs_disasm_iter(capstone_get_instance(), &codeCursor, &allowedToProcess, &addr, insn);
	assert(status);

	//If it's an if-style JUMP, record that we can go further in this function
	if (platform_isIf(*insn))
	{
		const uint8_t* jumpLoc = (const uint8_t*)platform_getRelAddr(*insn);
		if (jumpLoc > furthestKnownJump) furthestKnownJump = jumpLoc;
		return true;
	}
	//If it's a RETURN, continue only if we are allowed to JUMP to later
	else if (insn_in_group(*insn, cs_group_type::CS_GRP_RET))
	{
		return furthestKnownJump > codeCursor;
	}
	//Normal instructions: just continue parsing
	else return true;
}

FunctionBytecodeWalker::FunctionBytecodeWalker(const FunctionBytecodeWalker& cpy)
{
	this->insn = nullptr;
	*this = cpy;
}

FunctionBytecodeWalker::FunctionBytecodeWalker(FunctionBytecodeWalker&& mov)
{
	this->insn = nullptr;
	*this = mov;
}

FunctionBytecodeWalker& FunctionBytecodeWalker::operator=(const FunctionBytecodeWalker& cpy)
{
	this->furthestKnownJump = cpy.furthestKnownJump;
	this->codeCursor = cpy.codeCursor;

	if (this->insn) cs_free(this->insn, 1);
	this->insn = cs_malloc(capstone_get_instance()); //Note that this instruction will be invalid until the next time advance() is called

	return *this;
}

FunctionBytecodeWalker& FunctionBytecodeWalker::operator=(FunctionBytecodeWalker&& mov)
{
	this->furthestKnownJump = mov.furthestKnownJump;
	this->codeCursor        = mov.codeCursor;

	if (this->insn) cs_free(this->insn, 1);
	this->insn              = mov.insn;

	mov.furthestKnownJump = nullptr;
	mov.codeCursor = nullptr;
	mov.insn = nullptr;

	return *this;
}
