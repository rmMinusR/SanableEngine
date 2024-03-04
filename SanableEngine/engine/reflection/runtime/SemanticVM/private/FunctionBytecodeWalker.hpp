#pragma once

#include "capstone/capstone.h"

class FunctionBytecodeWalker
{
private:
	const uint8_t* furthestKnownJump;

public:
	FunctionBytecodeWalker(const uint8_t* data);
	FunctionBytecodeWalker(void(*fn)());
	~FunctionBytecodeWalker();

	bool advance();

	const uint8_t* codeCursor; //RIP equivalent
	cs_insn* insn; //Last-parsed instruction

	FunctionBytecodeWalker(const FunctionBytecodeWalker& cpy);
	FunctionBytecodeWalker(FunctionBytecodeWalker&& mov);
	FunctionBytecodeWalker& operator=(const FunctionBytecodeWalker& cpy);
	FunctionBytecodeWalker& operator=(FunctionBytecodeWalker&& mov);
};
