#include "DetectedConstants.hpp"

#include <cassert>
#include <functional>

#include "CapstoneWrapper.hpp"
#include "FunctionBytecodeWalker.hpp"
#include "SemanticVM.hpp"


DetectedConstants DetectedConstants::captureCtor(size_t objSize, void(*ctor)())
{
	//Setup VM
	SemanticVM vm;
	(*vm.canonicalState.registers[X86_REG_RBP]) = SemanticUnknown(sizeof(void*)); //Caller is indeterminate. TODO: 32-bit-on-64 support?
	(*vm.canonicalState.registers[X86_REG_RSP]) = SemanticKnownConst(-2 * sizeof(void*), sizeof(void*)); //TODO: This is a magic value, the size of one stack frame. Should be treated similarly to ThisPtr instead.
	(*vm.canonicalState.registers[X86_REG_RCX]) = SemanticThisPtr(0); //__thiscall: caller puts address of class object in rCX (see <https://en.wikibooks.org/wiki/X86_Disassembly/Calling_Conventions#THISCALL>)

	//Run
	vm.execFunc(ctor, nullptr);
	vm.canonicalState.debugPrintWorkingSet();
	printf("\n");

	//Ensure good output
	{
		auto* rsp = vm.canonicalState.registers[X86_REG_RSP]->tryGetKnownConst();
		assert(rsp && rsp->value == 0);
	}

	//Read from state.thisMemory into DetectedConstants
	DetectedConstants out(objSize);
	for (size_t i = 0; i < objSize; ++i)
	{
		SemanticValue _byte = vm.canonicalState.getMemory(SemanticThisPtr{ i }, 1);
		if (auto* byte = _byte.tryGetKnownConst())
		{
			out.usage[i] = true;
			out.bytes[i] = byte->value;
		}
	}
	return out;
}
