#pragma once

#include <cstdint>
#include <map>

#include "capstone/capstone.h"

#include "SemanticValue.hpp"


struct MachineState
{
public:
	constexpr static size_t nRegisters = X86_REG_ENDING;

private:
	GeneralValue __registerStorage[nRegisters]; //RAW DATA. DO NOT ACCESS DIRECTLY.
	GeneralValue* __registerMappings[nRegisters]; //You can access this if you want, but probably use "registers" instead
public:
	//Safety: Redefine registers as immutable
	GeneralValue* const* const registers = __registerMappings;
	GeneralValue decodeMemAddr(const x86_op_mem&) const;

private:
	std::map<uint8_t*, GeneralValue> memory; //Will never be SemanticUnknown
	std::map<size_t, GeneralValue> thisMemory; //Memory of the "this" object
public:
	MachineState();

	void reset();
	GeneralValue getOperand(const cs_insn* insn, size_t index) const;
	void setOperand(const cs_insn* insn, size_t index, GeneralValue value);
	GeneralValue getMemory(void*              location, size_t size) const;
	GeneralValue getMemory(GeneralValue       location, size_t size) const;
	GeneralValue getMemory(SemanticKnownConst location, size_t size) const;
	GeneralValue getMemory(SemanticThisPtr    location, size_t size) const;
	void setMemory(void*              location, GeneralValue value, size_t size); //"size" only used if value is unknown
	void setMemory(GeneralValue       location, GeneralValue value, size_t size);
	void setMemory(SemanticKnownConst location, GeneralValue value, size_t size);
	void setMemory(SemanticThisPtr    location, GeneralValue value, size_t size);
};
