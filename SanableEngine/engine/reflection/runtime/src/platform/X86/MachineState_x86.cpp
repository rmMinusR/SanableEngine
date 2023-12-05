#include "MachineState.hpp"

#include <cassert>
#include <optional>

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
	for (auto& reg : __registerStorage) reg = SemanticUnknown(0);
}

SemanticValue MachineState::decodeMemAddr(const x86_op_mem& mem) const
{
	std::optional<SemanticValue> addr; //TODO: Return GeneralValue, account for ThisPtr
#define addValue(val) (addr = addr.value_or(SemanticKnownConst(0, sizeof(void*))) + (val))
	if (mem.base  != X86_REG_INVALID) addValue( *registers[mem.base] );
	if (mem.index != X86_REG_INVALID) addValue( *registers[mem.index] * SemanticKnownConst(mem.scale, sizeof(void*)) );
	assert(mem.segment == X86_REG_INVALID); //FIXME: I'm not dealing with that right now
	addValue( SemanticKnownConst(mem.disp, sizeof(void*)) );
	return addr.value();
}

SemanticValue MachineState::getOperand(const cs_insn* insn, size_t index) const
{
	assert(index < insn->detail->x86.op_count);
	assert(insn->detail->x86.operands[index].size);
	
	//Special case: LEA (relative addresses)
	if (insn->id == x86_insn::X86_INS_LEA && index == 1)
	{
		return SemanticKnownConst((uint_addr_t)platform_getRelAddr(*insn), insn->detail->x86.operands[index].size);
	}
	//Special case: jmp aka branch (relative addresses)
	else if (carray_contains(insn->detail->groups, insn->detail->groups_count, x86_insn_group::X86_GRP_BRANCH_RELATIVE))
	{
		return SemanticKnownConst((uint_addr_t)platform_getRelAddr(*insn), insn->detail->x86.operands[index].size);
	}
	
	//General case
	cs_x86_op op = insn->detail->x86.operands[index];
	SemanticValue out;
	switch (op.type)
	{
	case x86_op_type::X86_OP_REG:
		out = *registers[op.reg];
		break;

	case x86_op_type::X86_OP_IMM:
		out = SemanticKnownConst(op.imm, op.size); //Completely legal: almost every machine out there stores negatives as two's compliment
		break;

	case x86_op_type::X86_OP_MEM:
		out = getMemory(decodeMemAddr(op.mem), op.size);
		break;

	default:
		assert(false);
		break;
	}
	out.resize(op.size);
	return out;
}

void MachineState::setOperand(const cs_insn* insn, size_t index, SemanticValue value)
{
	assert(index < insn->detail->x86.op_count);
	
	cs_x86_op op = insn->detail->x86.operands[index];

	//General case
	switch (op.type)
	{
	case x86_op_type::X86_OP_REG:
		*registers[op.reg] = value;
		return;

	case x86_op_type::X86_OP_MEM:
		setMemory(decodeMemAddr(op.mem), value, op.size);
		return;
	
	default:
		assert(false);
	}
}

void MachineState::stackPush(SemanticValue value)
{
	SemanticKnownConst& rsp = *registers[X86_REG_RSP]->tryGetKnownConst();
	rsp.value -= value.getSize(); //Make space on stack
	setMemory(rsp, value, value.getSize()); //Write to stack
}

SemanticValue MachineState::stackPop(size_t nBytes)
{
	SemanticKnownConst& rsp = *registers[X86_REG_RSP]->tryGetKnownConst();
	SemanticValue out = getMemory(rsp, nBytes); //Read from stack
	rsp.value += nBytes; //Reclaim space on stack
	return out;
}
