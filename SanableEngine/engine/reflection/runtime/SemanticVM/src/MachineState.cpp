#include "MachineState.hpp"

#include <cassert>

SemanticValue MachineState::getMemory(void*              location, size_t size) const { return constMemory.get(location       , size); }
SemanticValue MachineState::getMemory(SemanticThisPtr    location, size_t size) const { return thisMemory .get(location.offset, size); }
SemanticValue MachineState::getMemory(SemanticKnownConst location, size_t size) const { return constMemory.get(location.value , size); }

SemanticValue MachineState::getMemory(SemanticValue _location, size_t size) const
{
	     if (_location.isUnknown()) return SemanticUnknown(size);
	else if (SemanticKnownConst* loc = _location.tryGetKnownConst()) return getMemory(*loc, size);
	else if (SemanticThisPtr   * loc = _location.tryGetThisPtr   ()) return getMemory(*loc, size);
	else
	{
		assert(false && "Unknown SemanticValue form");
		return SemanticUnknown(size);
	}
}

void MachineState::setMemory(void*              location, SemanticValue value, size_t size) { return constMemory.set(location       , value, size); }
void MachineState::setMemory(SemanticThisPtr    location, SemanticValue value, size_t size) { return thisMemory .set(location.offset, value, size); }
void MachineState::setMemory(SemanticKnownConst location, SemanticValue value, size_t size) { return constMemory.set(location.value , value, size); }

void MachineState::setMemory(SemanticValue _location, SemanticValue value, size_t size)
{
	     if (SemanticKnownConst* loc = _location.tryGetKnownConst()) setMemory(*loc, value, size);
	else if (SemanticThisPtr   * loc = _location.tryGetThisPtr   ()) setMemory(*loc, value, size);
	else if (_location.isUnknown()) assert(false && "Cannot write to an indeterminate memory address! This would make the entire memory indeterminate.");
	else assert(false && "Unknown SemanticValue form");
}
