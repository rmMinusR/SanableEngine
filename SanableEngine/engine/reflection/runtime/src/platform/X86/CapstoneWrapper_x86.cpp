#include "CtorCapture.hpp"

#include "CapstoneWrapper.hpp"
#include "FunctionBytecodeWalker.hpp"

void* platform_getRelAddr(const cs_insn& insn)
{
	return reinterpret_cast<void*>((uint_addr_t)X86_REL_ADDR(insn));
}

void_func_ptr platform_getRelFunc(const cs_insn& insn)
{
	return reinterpret_cast<void(*)()>((uint_addr_t)X86_REL_ADDR(insn));
}

DetectedConstants platform_captureConstants(size_t objSize, void(*ctor)())
{
	
}
