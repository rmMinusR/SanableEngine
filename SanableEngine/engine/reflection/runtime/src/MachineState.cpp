#include "MachineState.hpp"

#include <cassert>

SemanticValue MachineState::getMemory(void* _location, size_t size) const
{
	uint8_t* location = (uint8_t*)_location;

	//Sanity check first: Entire byte-string is present and same type
	SemanticValue::Type type;
	{
		//If any bytes are missing, abort
		for (size_t i = 0; i < size; ++i) if (memory.find(location+i) == memory.end()) return SemanticUnknown(size);

		type = memory.at(location).getType();

		//If bytes are different types, abort
		for (size_t i = 1; i < size; ++i) if (type != memory.at(location+i).getType()) return SemanticUnknown(size);

		//If ThisPtr, ensure same offset
		if (type == SemanticValue::Type::ThisPtr)
		{
			decltype(SemanticThisPtr::offset) expectedOffset = memory.at(location).tryGetThisPtr()->offset;
			for (size_t i = 1; i < size; ++i)
			{
				auto* pThis = memory.at(location+i).tryGetThisPtr();
				if (pThis->offset != expectedOffset) return SemanticUnknown(size); //Something went *very* wrong, since we're shearing offsets on a magic value
			}
		}
	}

	if (type == SemanticValue::Type::KnownConst)
	{
		//Try to load value at address, if it is fully present as a constant
		SemanticKnownConst knownConst(0, size);
		for (size_t i = 0; i < size; ++i)
		{
			knownConst.byte(i) = memory.at(location+i).tryGetKnownConst()->value;
		}
		return knownConst;
	}
	else if (type == SemanticValue::Type::ThisPtr)
	{
		return SemanticThisPtr(memory.at(location).tryGetThisPtr()->offset);
	}
	else
	{
		//Something went very, very wrong
		assert(false);
		return SemanticUnknown(size);
	}
}

SemanticValue MachineState::getMemory(SemanticThisPtr _location, size_t size) const
{
	size_t location = _location.offset;

	//Sanity check first: Entire byte-string is present and same type
	SemanticValue::Type type;
	{
		//If any bytes are missing, abort
		for (size_t i = 0; i < size; ++i) if (thisMemory.find(location+i) == thisMemory.end()) return SemanticUnknown(size);

		type = thisMemory.at(location).getType();

		//If bytes are different types, abort
		for (size_t i = 1; i < size; ++i) if (type != thisMemory.at(location+i).getType()) return SemanticUnknown(size);

		//If ThisPtr, ensure same offset
		if (type == SemanticValue::Type::ThisPtr)
		{
			decltype(SemanticThisPtr::offset) expectedOffset = thisMemory.at(location).tryGetThisPtr()->offset;
			for (size_t i = 1; i < size; ++i)
			{
				auto* pThis = thisMemory.at(location+i).tryGetThisPtr();
				if (pThis->offset != expectedOffset) return SemanticUnknown(size); //Something went *very* wrong, since we're shearing offsets on a magic value
			}
		}
	}

	if (type == SemanticValue::Type::KnownConst)
	{
		//Try to load value at address, if it is fully present as a constant
		SemanticKnownConst knownConst(0, size);
		for (size_t i = 0; i < size; ++i)
		{
			knownConst.byte(i) = thisMemory.at(location+i).tryGetKnownConst()->value;
		}
		return knownConst;
	}
	else if (type == SemanticValue::Type::ThisPtr)
	{
		return SemanticThisPtr(thisMemory.at(location).tryGetThisPtr()->offset);
	}
	else
	{
		//Something went very, very wrong
		assert(false);
		return SemanticUnknown(size);
	}
}

SemanticValue MachineState::getMemory(SemanticKnownConst location, size_t size) const
{
	return getMemory((uint8_t*)location.value, size);
}

SemanticValue MachineState::getMemory(SemanticValue _location, size_t size) const
{
	     if (_location.isUnknown()) return SemanticUnknown(size);
	else if (SemanticKnownConst* loc = _location.tryGetKnownConst()) return getMemory(*loc, size);
	else if (SemanticThisPtr   * loc = _location.tryGetThisPtr   ()) return getMemory(*loc, size);
	else
	{
		assert(false);
		return SemanticUnknown(size);
	}
}

void MachineState::setMemory(void* _location, SemanticValue value, size_t size)
{
	uint8_t* location = (uint8_t*)_location;

	if (auto* val = value.tryGetKnownConst())
	{
		for (size_t i = 0; i < val->size; ++i) memory.insert_or_assign(location+i, SemanticKnownConst(val->byte(i), 1));
	}
	else if (value.tryGetThisPtr())
	{
		//Just hope there's no shearing
		for (size_t i = 0; i < sizeof(void*); ++i) memory.insert_or_assign(location+i, value);
	}
	else
	{
		//Value was unknown
		for (size_t i = 0; i < size; ++i) memory.erase(location+i);
	}
}

void MachineState::setMemory(SemanticThisPtr _location, SemanticValue value, size_t size)
{
	size_t location = _location.offset;

	if (auto* val = value.tryGetKnownConst())
	{
		for (size_t i = 0; i < val->size; ++i) thisMemory.insert_or_assign(location+i, SemanticKnownConst(val->byte(i), 1));
	}
	else if (value.tryGetThisPtr())
	{
		//Just hope there's no shearing
		for (size_t i = 0; i < sizeof(void*); ++i) thisMemory.insert_or_assign(location+i, value);
	}
	else
	{
		//Value was unknown
		for (size_t i = 0; i < size; ++i) thisMemory.erase(location+i);
	}
}

void MachineState::setMemory(SemanticKnownConst location, SemanticValue value, size_t size)
{
	setMemory((uint8_t*)location.value, value, size);
}

void MachineState::setMemory(SemanticValue _location, SemanticValue value, size_t size)
{
	//if (_location.isUnknown()) return;
	     if (SemanticKnownConst* loc = _location.tryGetKnownConst()) setMemory(*loc, value, size);
	else if (SemanticThisPtr   * loc = _location.tryGetThisPtr   ()) setMemory(*loc, value, size);
	else
	{
		assert(false);
	}
}
