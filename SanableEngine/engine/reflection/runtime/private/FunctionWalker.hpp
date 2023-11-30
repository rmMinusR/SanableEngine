#pragma once

#include "capstone/capstone.h"

class FunctionWalker
{
private:
	const uint8_t* furthestKnownJump;

public:
	FunctionWalker(const uint8_t* data);
	FunctionWalker(void(*fn)());
	~FunctionWalker();

	bool advance();

	const uint8_t* codeCursor; //RIP equivalent
	cs_insn* insn; //Last-parsed instruction
};
