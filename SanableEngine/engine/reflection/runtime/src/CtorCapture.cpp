#include "CtorCapture.hpp"

#include <cassert>

#include "CapstoneWrapper.hpp"
#include "FunctionBytecodeWalker.hpp"
#include "SemanticVM.hpp"

DetectedConstants capture_utils::_captureVtablesInternal(size_t objSize, void(*thunk)(), const std::vector<void(*)()>& allocators, const std::vector<void(*)()>& nofill)
{
	//Simulate
	MachineState canonicalState(true);
	SemanticVM vm;
	vm.execFunc(canonicalState, thunk, allocators, nofill);

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
