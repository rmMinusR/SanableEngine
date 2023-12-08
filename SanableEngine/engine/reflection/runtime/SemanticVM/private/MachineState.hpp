#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <optional>

#include "capstone/capstone.h"

#include "SemanticValue.hpp"
#include "VMMemory.hpp"


struct MachineState
{
public:
	constexpr static size_t nRegisters = X86_REG_ENDING;

private:
	std::map<x86_reg, SemanticValue> __registerStorage;
	x86_reg __registerMappings[nRegisters];
public:
	SemanticValue decodeMemAddr(const x86_op_mem&) const;
	SemanticValue getRegister(x86_reg id) const;
	void setRegister(x86_reg id, SemanticValue val);
	enum class FlagIDs
	{
		Carry = 0,
		Parity = 2,
		AuxCarry = 4,
		Zero = 6,
		Sign = 7
	};

private:
	VMMemory constMemory; //General memory with known constant address
	VMMemory thisMemory; //Memory of the "this" object
public:
	MachineState();

	void reset();

	SemanticValue getOperand(const cs_insn* insn, size_t index) const;
	void setOperand(const cs_insn* insn, size_t index, SemanticValue value);

	SemanticValue getMemory(void*              location, size_t size) const;
	SemanticValue getMemory(SemanticValue      location, size_t size) const;
	SemanticValue getMemory(SemanticKnownConst location, size_t size) const;
	SemanticValue getMemory(SemanticThisPtr    location, size_t size) const;
	void setMemory(void*              location, SemanticValue value, size_t size); //"size" only used if value is unknown
	void setMemory(SemanticValue      location, SemanticValue value, size_t size);
	void setMemory(SemanticKnownConst location, SemanticValue value, size_t size);
	void setMemory(SemanticThisPtr    location, SemanticValue value, size_t size);

	void stackPush(SemanticValue value);
	SemanticValue stackPop(size_t nBytes);

	bool conditionMet(const cs_insn& insn) const; //Given a conditional instruction (ie. JNE), is the specified condition met? Defined per-platform.

	//size_t countStackFrames() const;
	//using StackVisitor = std::function<void(const SemanticValue& rbp, const SemanticValue& returnLocation)>;
	//void unwindStack(const StackVisitor& visitor) const;

	int debugPrintWorkingSet() const; //On register-based machines, prints critical registers. On stack-based machines, prints stack head.

	static MachineState merge(const std::vector<const MachineState*>& divergentStates);
};
