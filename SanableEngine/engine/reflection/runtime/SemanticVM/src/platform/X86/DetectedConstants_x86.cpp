#include "DetectedConstants.hpp"

#include <cassert>
#include <functional>

#include "CapstoneWrapper.hpp"
#include "FunctionBytecodeWalker.hpp"
#include "SemanticVM.hpp"


DetectedConstants DetectedConstants::captureCtor(size_t objSize, void(*ctor)())
{
	//Setup VM
	MachineState canonicalState(true);
	canonicalState.setRegister(X86_REG_RCX, SemanticThisPtr(0) ); //__thiscall: caller puts address of class object in rCX (see <https://en.wikibooks.org/wiki/X86_Disassembly/Calling_Conventions#THISCALL>)

	//Run
	SemanticVM vm;
	vm.execFunc(canonicalState, ctor, {}, {});

	//Read from state.thisMemory into DetectedConstants
	DetectedConstants out(objSize);
	for (size_t i = 0; i < objSize; ++i)
	{
		SemanticValue _byte = canonicalState.getMemory(SemanticThisPtr{ i }, 1);
		if (auto* byte = _byte.tryGetKnownConst())
		{
			out.usage[i] = true;
			out.bytes[i] = byte->value;
		}
	}
	return out;
}
