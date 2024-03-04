#include "ThunkUtils.hpp"

#include <cassert>

#include "CapstoneWrapper.hpp"
#include "FunctionBytecodeWalker.hpp"
#include "SemanticVM.hpp"

ptrdiff_t _captureCastOffset(const DetectedConstants& image, void(*castThunk)())
{
	MachineState canonicalState(true);
	for (int i = 0; i < image.bytes.size(); ++i)
	{
		if (image.usage[i]) canonicalState.setMemory(SemanticThisPtr(i), SemanticKnownConst(image.bytes[i], 1, false), 1);
	}

	//Simulate
	SemanticVM::execFunc(canonicalState, castThunk, {}, {});

	assert(false); //TODO implement
	return ptrdiff_t();
}

DetectedConstants _captureVtablesInternal(size_t objSize, void(*thunk)(), const std::vector<void(*)()>& allocators, const std::vector<void(*)()>& nofill)
{
	//Simulate
	MachineState canonicalState(true);
	SemanticVM::execFunc(canonicalState, thunk, allocators, nofill);

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
