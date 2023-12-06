#include "VMMemory.hpp"

#include <cassert>

void VMMemory::reset()
{
	memory.clear();
}

void VMMemory::set(void* _location, SemanticValue value, size_t size)
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
	else if (value.isUnknown())
	{
		//Value was unknown, but mark that we wrote it
		for (size_t i = 0; i < size; ++i) memory.insert_or_assign(location+i, value);
	}
	else assert(false);
}

SemanticValue VMMemory::get(void* _location, size_t size) const
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
	else if (type == SemanticValue::Type::Unknown)
	{
		return SemanticUnknown(size);
	}
	else
	{
		//Something went very, very wrong
		assert(false);
		return SemanticUnknown(size);
	}
}
