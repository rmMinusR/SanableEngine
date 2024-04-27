#include "MachineState.hpp"
#include "SemanticVM.hpp"

#include <cassert>

VMMemory& MachineState::magicMemory(SemanticMagic::id_t id)
{
	if (!magics.count(id)) magics.emplace(std::make_pair(id, VMMemory()));
	return magics.at(id);
}

SemanticValue MachineState::getMemory(void* location, size_t size, bool tryHostMemory) const
{
	SemanticValue out = constMemory.get(location, size);
	if (canReadHostMemory && tryHostMemory && out.isUnknown())
	{
		if (SemanticVM::debug) printf("Read host memory: %p[%i]\n", location, (int)size);
		SemanticKnownConst c(0, size, false);
		memcpy(&c.value, location, size);
		return c;
	}
	else return out;
}

//SemanticValue MachineState::getMemory(void*              location, size_t size) const { return constMemory.get(location       , size); }
SemanticValue MachineState::getMemory(SemanticMagic      location, size_t size) const { return magics.count(location.id) ? magics.at(location.id).get(location.offset, size) : SemanticUnknown(0); }
SemanticValue MachineState::getMemory(SemanticKnownConst location, size_t size) const { return getMemory((void*)location.value, size, location.isPositionIndependentAddr); }

SemanticValue MachineState::getMemory(SemanticValue _location, size_t size) const
{
	     if (_location.isUnknown()) return SemanticUnknown(size);
	else if (SemanticKnownConst* loc = _location.tryGetKnownConst()) return getMemory(*loc, size);
	else if (SemanticMagic     * loc = _location.tryGetMagic     ()) return getMemory(*loc, size);
	else
	{
		assert(false && "Unknown SemanticValue form");
		return SemanticUnknown(size);
	}
}

void MachineState::setMemory(void*              location, SemanticValue value, size_t size) { return constMemory             .set(location       , value, size); }
void MachineState::setMemory(SemanticMagic      location, SemanticValue value, size_t size) { return magicMemory(location.id).set(location.offset, value, size); }
void MachineState::setMemory(SemanticKnownConst location, SemanticValue value, size_t size) { return constMemory             .set(location.value , value, size); }

void MachineState::setMemory(SemanticValue _location, SemanticValue value, size_t size)
{
	     if (SemanticKnownConst* loc = _location.tryGetKnownConst()) setMemory(*loc, value, size);
	else if (SemanticMagic     * loc = _location.tryGetMagic     ()) setMemory(*loc, value, size);
	else if (_location.isUnknown()) assert(false && "Cannot write to an indeterminate memory address! This would make the entire memory indeterminate.");
	else assert(false && "Unknown SemanticValue form");
}
