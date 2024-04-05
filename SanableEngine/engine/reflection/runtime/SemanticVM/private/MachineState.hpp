#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <optional>

#include "capstone/capstone.h"

#include "CapstoneWrapper.hpp"
#include "SemanticValue.hpp"
#include "VMMemory.hpp"


struct MachineState
{
public:
	constexpr static size_t nRegisters = X86_REG_ENDING;

private:
	std::map<x86_reg, SemanticValue> __registerStorage;
	x86_reg __registerMappings[nRegisters];
	bool canReadHostMemory;
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
		Sign = 7,
		Trap = 8,
		InterruptEnable = 9,
		Direction = 10,
		Overflow = 11
	};

private:
	VMMemory constMemory; //General memory with known constant address
	std::map<SemanticMagic::id_t, VMMemory> magics; //Memory of dynamic allocations and the "this" object
	VMMemory& magicMemory(SemanticMagic::id_t id); //Creates if doesn't exist
public:
	MachineState(bool canReadHostMemory);

	void reset();

	SemanticValue getOperand(const cs_insn* insn, size_t index) const;
	void setOperand(const cs_insn* insn, size_t index, SemanticValue value);

	SemanticValue getMemory(void*              location, size_t size, bool tryHostMemory) const;
	SemanticValue getMemory(SemanticValue      location, size_t size) const;
	SemanticValue getMemory(SemanticKnownConst location, size_t size) const;
	SemanticValue getMemory(SemanticMagic      location, size_t size) const;
	void setMemory(void*              location, SemanticValue value, size_t size); //"size" only used if value is unknown
	void setMemory(SemanticValue      location, SemanticValue value, size_t size);
	void setMemory(SemanticKnownConst location, SemanticValue value, size_t size);
	void setMemory(SemanticMagic      location, SemanticValue value, size_t size);

	uint_addr_t getInsnPtr() const;
	void setInsnPtr(uint_addr_t val);

	void stackPush(SemanticValue value);
	SemanticValue stackPop(size_t nBytes);
	void pushStackFrame(SemanticKnownConst fp);
	SemanticValue popStackFrame();

	std::optional<bool> isConditionMet(unsigned int insnId) const; //Check if the condition described by a branch instruction is met, or nullopt if it is indeterminate

	//size_t countStackFrames() const;
	//using StackVisitor = std::function<void(const SemanticValue& rbp, const SemanticValue& returnLocation)>;
	//void unwindStack(const StackVisitor& visitor) const;

	int debugPrintWorkingSet() const; //On register-based machines, prints critical registers. On stack-based machines, prints stack head.
	const char* checkGood() const; //Check that all critical pieces (stack and instruction pointers) are valid. If an error has occurred, returns an error string.
	void requireGood() const; //Like checkGood, but asserts on failure

	static MachineState merge(const std::vector<const MachineState*>& divergentStates);
};
