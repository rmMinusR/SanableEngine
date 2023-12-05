#include "MachineState.hpp"

template<typename T1, typename T2, typename T3, typename T4>
GeneralValue GeneralValue_doMathOp(GeneralValue arg1, GeneralValue arg2,
	T1 funcConstConst, T2 funcConstThis, T3 funcThisConst, T4 funcThisThis)
{
		 if (std::holds_alternative<SemanticUnknown   >(arg1) || std::holds_alternative<SemanticUnknown   >(arg2)) return SemanticUnknown();
	else if (std::holds_alternative<SemanticKnownConst>(arg1) && std::holds_alternative<SemanticKnownConst>(arg2))
	{
		auto a = std::get<SemanticKnownConst>(arg1);
		auto b = std::get<SemanticKnownConst>(arg2);
		assert(a.size == b.size);
		return funcConstConst(a, b);
	}
	else if (std::holds_alternative<SemanticKnownConst>(arg1) && std::holds_alternative<SemanticThisPtr   >(arg2)) return funcConstThis (std::get<SemanticKnownConst>(arg1), std::get<SemanticThisPtr   >(arg2));
	else if (std::holds_alternative<SemanticThisPtr   >(arg1) && std::holds_alternative<SemanticKnownConst>(arg2)) return funcThisConst (std::get<SemanticThisPtr   >(arg1), std::get<SemanticKnownConst>(arg2));
	else if (std::holds_alternative<SemanticThisPtr   >(arg1) && std::holds_alternative<SemanticThisPtr   >(arg2)) return funcThisThis  (std::get<SemanticThisPtr   >(arg1), std::get<SemanticThisPtr   >(arg2));
	else { assert(false); return GeneralValue(); }
}

GeneralValue operator+(const GeneralValue& lhs, const GeneralValue& rhs)
{
	return GeneralValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.value + arg2.value, arg1.size); },
		[](SemanticKnownConst arg1, SemanticThisPtr    arg2) { return SemanticThisPtr{ arg1.value + arg2.offset }; },
		[](SemanticThisPtr    arg1, SemanticKnownConst arg2) { return SemanticThisPtr{ arg1.offset + arg2.value }; },
		[](SemanticThisPtr    arg1, SemanticThisPtr    arg2) { return SemanticUnknown(); }
	);
}

GeneralValue operator-(const GeneralValue& lhs, const GeneralValue& rhs)
{
	return GeneralValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.value - arg2.value, arg1.size); },
		[](SemanticKnownConst arg1, SemanticThisPtr    arg2) { return SemanticUnknown(); },
		[](SemanticThisPtr    arg1, SemanticKnownConst arg2) { return SemanticThisPtr{ arg1.offset - arg2.value }; },
		[](SemanticThisPtr    arg1, SemanticThisPtr    arg2) { return SemanticKnownConst(arg1.offset - arg2.offset, sizeof(void*)); }
	);
}

GeneralValue operator*(const GeneralValue& lhs, const GeneralValue& rhs)
{
	return GeneralValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.value * arg2.value, arg1.size); },
		[](SemanticKnownConst arg1, SemanticThisPtr    arg2) { return SemanticUnknown(); },
		[](SemanticThisPtr    arg1, SemanticKnownConst arg2) { return SemanticUnknown(); },
		[](SemanticThisPtr    arg1, SemanticThisPtr    arg2) { return SemanticUnknown(); }
	);
}


GeneralValue MachineState::getMemory(void* _location, size_t size) const
{
	uint8_t* location = (uint8_t*)_location;

	//Sanity check first: Entire byte-string is present and same type
	auto it = memory.find(location);
	if (it == memory.end()) return SemanticUnknown(); //Nothing found. Abort!
	GeneralValue dummy = it->second;
	for (size_t i = 0; i < size; ++i)
	{
		it = memory.find(location+i);
		if (it == memory.end()) return SemanticUnknown(); //Some bytes missing. Abort!
		if (dummy.index() != it->second.index()) return SemanticUnknown(); //Some bytes were mismatched types. Abort!
		if (std::holds_alternative<SemanticThisPtr>(dummy) && std::get<SemanticThisPtr>(dummy).offset != std::get<SemanticThisPtr>(it->second).offset) return SemanticUnknown(); //Something went *very* wrong, since we're shearing offsets on a magic value
	}

	if (std::holds_alternative<SemanticKnownConst>(it->second))
	{
		//Try to load value at address, if it is fully present as a constant
		SemanticKnownConst knownConst(0, size);
		for (size_t i = 0; i < size; ++i)
		{
			knownConst.byte(i) = std::get<SemanticKnownConst>(memory.at(location+i)).value;
		}
		return knownConst;
	}
	else if (std::holds_alternative<SemanticThisPtr>(it->second))
	{
		//Just hope there's no shearing
		return std::get<SemanticThisPtr>(it->second);
	}
	else
	{
		//Something went very, very wrong
		assert(false);
		return SemanticUnknown();
	}
}

GeneralValue MachineState::getMemory(SemanticThisPtr _location, size_t size) const
{
	size_t location = _location.offset;

	//Sanity check first: Entire byte-string is present and same type
	auto it = thisMemory.find(location);
	if (it == thisMemory.end()) return SemanticUnknown(); //Nothing found. Abort!
	GeneralValue dummy = it->second;
	for (size_t i = 0; i < size; ++i)
	{
		it = thisMemory.find(location+i);
		if (it == thisMemory.end()) return SemanticUnknown(); //Some bytes missing. Abort!
		if (dummy.index() != it->second.index()) return SemanticUnknown(); //Some bytes were mismatched types. Abort!
		if (std::holds_alternative<SemanticThisPtr>(dummy) && std::get<SemanticThisPtr>(dummy).offset != std::get<SemanticThisPtr>(it->second).offset) return SemanticUnknown(); //Something went *very* wrong, since we're shearing offsets on a magic value
	}

	if (std::holds_alternative<SemanticKnownConst>(it->second))
	{
		//Try to load value at address, if it is fully present as a constant
		SemanticKnownConst knownConst(0, size);
		for (size_t i = 0; i < size; ++i)
		{
			knownConst.byte(i) = std::get<SemanticKnownConst>(thisMemory.at(location+i)).value;
		}
		return knownConst;
	}
	else if (std::holds_alternative<SemanticThisPtr>(it->second))
	{
		//Just hope there's no shearing
		return std::get<SemanticThisPtr>(it->second);
	}
	else
	{
		//Something went very, very wrong
		assert(false);
		return SemanticUnknown();
	}
}

GeneralValue MachineState::getMemory(SemanticKnownConst location, size_t size) const
{
	return getMemory((uint8_t*)location.value, size);
}

GeneralValue MachineState::getMemory(GeneralValue _location, size_t size) const
{
	     if (std::holds_alternative<SemanticUnknown   >(_location)) return SemanticUnknown();
	else if (std::holds_alternative<SemanticKnownConst>(_location)) return getMemory(std::get<SemanticKnownConst>(_location), size);
	else if (std::holds_alternative<SemanticThisPtr   >(_location)) return getMemory(std::get<SemanticThisPtr   >(_location), size);
	else
	{
		assert(false);
		return GeneralValue();
	}
}

void MachineState::setMemory(void* _location, GeneralValue value, size_t size)
{
	uint8_t* location = (uint8_t*)_location;

	if (std::holds_alternative<SemanticKnownConst>(value))
	{
		SemanticKnownConst val = std::get<SemanticKnownConst>(value);
		for (size_t i = 0; i < val.size; ++i) memory.insert_or_assign(location+i, SemanticKnownConst(val.byte(i), 1));
	}
	else if (std::holds_alternative<SemanticThisPtr>(value))
	{
		//Just hope there's no shearing
		for (size_t i = 0; i < sizeof(void*); ++i) memory.insert_or_assign(location+i, std::get<SemanticThisPtr>(value));
	}
	else
	{
		//Value was unknown
		for (size_t i = 0; i < size; ++i) memory.erase(location+i);
	}
}

void MachineState::setMemory(SemanticThisPtr _location, GeneralValue value, size_t size)
{
	size_t location = _location.offset;

	if (std::holds_alternative<SemanticKnownConst>(value))
	{
		SemanticKnownConst val = std::get<SemanticKnownConst>(value);
		for (size_t i = 0; i < val.size; ++i) thisMemory.insert_or_assign(location+i, SemanticKnownConst(val.byte(i), 1));
	}
	else if (std::holds_alternative<SemanticThisPtr>(value))
	{
		//Just hope there's no shearing
		for (size_t i = 0; i < sizeof(void*); ++i) thisMemory.insert_or_assign(location+i, std::get<SemanticThisPtr>(value));
	}
	else
	{
		//Value was unknown
		for (size_t i = 0; i < size; ++i) thisMemory.erase(location+i);
	}
}

void MachineState::setMemory(SemanticKnownConst location, GeneralValue value, size_t size)
{
	setMemory((uint8_t*)location.value, value, size);
}

void MachineState::setMemory(GeneralValue _location, GeneralValue value, size_t size)
{
	//if (std::holds_alternative<SemanticUnknown   >(_location)) return;
	     if (std::holds_alternative<SemanticKnownConst>(_location)) setMemory(std::get<SemanticKnownConst>(_location), value, size);
	else if (std::holds_alternative<SemanticThisPtr   >(_location)) setMemory(std::get<SemanticThisPtr   >(_location), value, size);
	else
	{
		assert(false);
	}
}
