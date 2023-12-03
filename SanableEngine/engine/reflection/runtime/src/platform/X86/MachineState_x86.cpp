#include "MachineState.hpp"

#include <cassert>

#include "CapstoneWrapper.hpp"


MachineState::MachineState()
{
	//Allow registers to share memory without heap allocation
	for (size_t i = 0; i < nRegisters; ++i) __registerMappings[i] = __registerStorage + i;

	//General-purpose registers share memory. Set those up.
	//See https://en.wikibooks.org/wiki/X86_Assembly/X86_Architecture#General-Purpose_Registers_(GPR)_-_16-bit_naming_conventions
#define DECL_REGISTER_GROUP_IDENTITY(suffix) __registerMappings[X86_REG_##suffix] = __registerMappings[X86_REG_R##suffix] = __registerMappings[X86_REG_E##suffix] = __registerStorage+(X86_REG_##suffix);
	DECL_REGISTER_GROUP_IDENTITY(AX);
	DECL_REGISTER_GROUP_IDENTITY(BX);
	DECL_REGISTER_GROUP_IDENTITY(CX);
	DECL_REGISTER_GROUP_IDENTITY(DX);
	DECL_REGISTER_GROUP_IDENTITY(SP);
	DECL_REGISTER_GROUP_IDENTITY(BP);
	DECL_REGISTER_GROUP_IDENTITY(SI);
	DECL_REGISTER_GROUP_IDENTITY(DI);
	
	//Set up all registers in unknown state
	reset();
};

void MachineState::reset()
{
	for (auto& reg : __registerStorage) reg = SemanticUnknown();
}

uint8_t* decodeMemAddr(x86_op_mem mem, GeneralValue* const* const registers)
{
	uint8_t* addr = nullptr; //TODO: Return GeneralValue, account for ThisPtr
	if (mem.base    != X86_REG_INVALID) if (GeneralValue reg = *registers[mem.base ]; std::holds_alternative<SemanticKnownConst>(reg)) addr += std::get<SemanticKnownConst>(reg).value;
	if (mem.index   != X86_REG_INVALID) if (GeneralValue reg = *registers[mem.index]; std::holds_alternative<SemanticKnownConst>(reg)) addr += std::get<SemanticKnownConst>(reg).value * mem.scale;
	assert(mem.segment == X86_REG_INVALID); //FIXME: I'm not dealing with that right now
	addr += mem.disp;
	return addr;
}

GeneralValue MachineState::getOperand(const cs_insn* insn, size_t index) const
{
	assert(index < insn->detail->x86.op_count);
	
	//Special case: LEA (relative addresses)
	if (insn->id == x86_insn::X86_INS_LEA && index == 1)
	{
		return SemanticKnownConst(X86_REL_ADDR(*insn), sizeof(void*));
	}
	//Special case: jmp aka branch (relative addresses)
	else if (carray_contains(insn->detail->groups, insn->detail->groups_count, x86_insn_group::X86_GRP_BRANCH_RELATIVE))
	{
		return SemanticKnownConst(X86_REL_ADDR(*insn), sizeof(void*));
	}
	
	//General case
	cs_x86_op op = insn->detail->x86.operands[index];
	switch (op.type)
	{
	case x86_op_type::X86_OP_REG:
		return *registers[op.reg];

	case x86_op_type::X86_OP_IMM:
		return SemanticKnownConst(op.imm, op.size); //Completely legal: every machine out there stores negatives as two's compliment

	case x86_op_type::X86_OP_MEM:
		return getMemory(decodeMemAddr(op.mem, registers), op.size);

	default:
		assert(false);
		return GeneralValue();
	}
}

void MachineState::setOperand(const cs_insn* insn, size_t index, GeneralValue value)
{
	assert(index < insn->detail->x86.op_count);
	
	cs_x86_op op = insn->detail->x86.operands[index];

	//Special case where operand should be treated as a pointer
	assert(!(insn->id == x86_insn::X86_INS_LEA && index == 0));

	//General case
	switch (op.type)
	{
	case x86_op_type::X86_OP_REG:
		*registers[op.reg] = value;
		return;

	case x86_op_type::X86_OP_MEM:
		setMemory(decodeMemAddr(op.mem, registers), value, op.size);
		return;
	
	default:
		assert(false);
	}
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
		for (size_t i = 0; i < sizeof(void*); ++i) memory.insert_or_assign(location+i, SemanticThisPtr());
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
		for (size_t i = 0; i < sizeof(void*); ++i) thisMemory.insert_or_assign(location+i, SemanticThisPtr());
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
	     if (std::holds_alternative<SemanticUnknown   >(_location)) return;
	else if (std::holds_alternative<SemanticKnownConst>(_location)) setMemory(std::get<SemanticKnownConst>(_location), value, size);
	else if (std::holds_alternative<SemanticThisPtr   >(_location)) setMemory(std::get<SemanticThisPtr   >(_location), value, size);
	else
	{
		assert(false);
	}
}
