#include "CtorCapture.hpp"

#include "CapstoneWrapper.hpp"
#include "FunctionBytecodeWalker.hpp"

void* platform_getRelAddr(const cs_insn& insn)
{
	return reinterpret_cast<void*>((uint_addr_t)X86_REL_ADDR(insn));
}

DetectedVtables platform_captureVtables(void(*ctor)())
{
	
}
