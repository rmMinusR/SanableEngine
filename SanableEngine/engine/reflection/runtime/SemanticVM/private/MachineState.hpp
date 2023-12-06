#pragma once

#include <cstdint>
#include <map>
#include <functional>

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
	SemanticValue decodeMemAddr(const x86_op_mem&) const;
	SemanticValue getRegister(x86_reg id) const;
	void setRegister(x86_reg id, SemanticValue val);

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

	void stackPush(SemanticValue value);
	SemanticValue stackPop(size_t nBytes);

	//size_t countStackFrames() const;
	//using StackVisitor = std::function<void(const SemanticValue& rbp, const SemanticValue& returnLocation)>;
	//void unwindStack(const StackVisitor& visitor) const;

	int debugPrintWorkingSet() const; //On register-based machines, prints critical registers. On stack-based machines, prints stack head.

	static MachineState merge(const std::vector<const MachineState*>& divergentStates);
};
