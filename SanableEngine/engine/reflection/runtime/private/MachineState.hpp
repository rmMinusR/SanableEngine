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
	SemanticValue __registerStorage[nRegisters]; //RAW DATA. DO NOT ACCESS DIRECTLY.
	SemanticValue* __registerMappings[nRegisters]; //You can access this if you want, but probably use "registers" instead
public:
	//Safety: Redefine registers as immutable
	SemanticValue* const* const registers = __registerMappings;
	SemanticValue decodeMemAddr(const x86_op_mem&) const;

private:
	std::map<uint8_t*, SemanticValue> memory; //Will never be SemanticUnknown
	std::map<size_t, SemanticValue> thisMemory; //Memory of the "this" object
public:
	MachineState();

	void reset();
	SemanticValue getOperand(const cs_insn* insn, size_t index) const;
	void setOperand(const cs_insn* insn, size_t index, SemanticValue value);
	SemanticValue getMemory(void*              location, size_t size) const;
	SemanticValue getMemory(SemanticValue       location, size_t size) const;
	SemanticValue getMemory(SemanticKnownConst location, size_t size) const;
	SemanticValue getMemory(SemanticThisPtr    location, size_t size) const;
	void setMemory(void*              location, SemanticValue value, size_t size); //"size" only used if value is unknown
	void setMemory(SemanticValue       location, SemanticValue value, size_t size);
	void setMemory(SemanticKnownConst location, SemanticValue value, size_t size);
	void setMemory(SemanticThisPtr    location, SemanticValue value, size_t size);
};
