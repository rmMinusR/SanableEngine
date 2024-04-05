#include "MachineState.hpp"

#include <cassert>
#include <optional>

#include "CapstoneWrapper.hpp"


MachineState::MachineState(bool canReadHostMemory) :
	canReadHostMemory(canReadHostMemory)
{
	//Allow registers to share memory without heap allocation
	for (size_t i = 0; i < nRegisters; ++i) __registerMappings[i] = (x86_reg)i;

	//General-purpose registers share memory. Set those up.
	//See https://en.wikibooks.org/wiki/X86_Assembly/X86_Architecture#General-Purpose_Registers_(GPR)_-_16-bit_naming_conventions
#define DECL_REGISTER_GROUP_IDENTITY(suffix) __registerMappings[X86_REG_##suffix] = __registerMappings[X86_REG_R##suffix] = __registerMappings[X86_REG_E##suffix] = X86_REG_##suffix;
	DECL_REGISTER_GROUP_IDENTITY(AX);
	DECL_REGISTER_GROUP_IDENTITY(BX);
	DECL_REGISTER_GROUP_IDENTITY(CX);
	DECL_REGISTER_GROUP_IDENTITY(DX);
	DECL_REGISTER_GROUP_IDENTITY(SP);
	DECL_REGISTER_GROUP_IDENTITY(BP);
	DECL_REGISTER_GROUP_IDENTITY(SI);
	DECL_REGISTER_GROUP_IDENTITY(DI);
	
	//Set up all registers in default state
	reset();
};

void MachineState::reset()
{
	__registerStorage.clear();

	constMemory.reset();
	magics.clear();
}

SemanticValue MachineState::decodeMemAddr(const x86_op_mem& mem) const
{
	std::optional<SemanticValue> addr;
#define addValue(val) (addr = addr.value_or(SemanticKnownConst(0, sizeof(void*), false)) + (val))
	if (mem.base  != X86_REG_INVALID) addValue( getRegister(mem.base) );
	if (mem.index != X86_REG_INVALID) addValue( getRegister(mem.index) * SemanticKnownConst(mem.scale, sizeof(void*), false) );
	assert(mem.segment == X86_REG_INVALID); //FIXME: I'm not dealing with that right now
	addValue( SemanticKnownConst(mem.disp, sizeof(void*), false) );
	return addr.value();
}

SemanticValue MachineState::getRegister(x86_reg id) const
{
	x86_reg mappedId = __registerMappings[id];
	auto it = __registerStorage.find(mappedId);
	return it!=__registerStorage.end() ? it->second : SemanticUnknown(0);
}

void MachineState::setRegister(x86_reg id, SemanticValue val)
{
	x86_reg mappedId = __registerMappings[id];
	if (mappedId == X86_REG_EFLAGS) //Needs special handling: must maintain that this is SemanticFlags
	{
		SemanticFlags _val;
			 if (val.isUnknown()) _val = SemanticFlags();
		else if (val.tryGetMagic()) _val = SemanticFlags(); //Treat ThisPtr as if it were unknown
		else if (auto* f = val.tryGetFlags()) _val = *f;
		else if (auto* c = val.tryGetKnownConst()) { _val.bits = c->value; _val.bitsKnown = ~(~0ull << (8*c->size) ); }
		else assert(false && "Unhandled value type");
		__registerStorage.insert_or_assign(mappedId, _val);
	}
	else
	{
		__registerStorage.insert_or_assign(mappedId, val);
	}
}

SemanticValue MachineState::getOperand(const cs_insn* insn, size_t index) const
{
	assert(index < insn->detail->x86.op_count);
	assert(insn->detail->x86.operands[index].size);
	
	//Special case: LEA (relative addresses)
	if (insn->id == x86_insn::X86_INS_LEA && index == 1)
	{
		return SemanticKnownConst((uint_addr_t)platform_getRelAddr(*insn), insn->detail->x86.operands[index].size, true);
	}
	//Special case: jmp aka branch (relative addresses)
	else if (insn_in_group(*insn, x86_insn_group::X86_GRP_BRANCH_RELATIVE))
	{
		return SemanticKnownConst((uint_addr_t)platform_getRelAddr(*insn), insn->detail->x86.operands[index].size, true);
	}
	
	//General case
	cs_x86_op op = insn->detail->x86.operands[index];
	SemanticValue out;
	switch (op.type)
	{
	case x86_op_type::X86_OP_REG:
		out = getRegister(op.reg);
		break;

	case x86_op_type::X86_OP_IMM:
		out = SemanticKnownConst(op.imm, op.size, false); //Completely legal: almost every machine out there stores negatives as two's compliment
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
		setRegister(op.reg, value);
		return;

	case x86_op_type::X86_OP_MEM:
		setMemory(decodeMemAddr(op.mem), value, op.size);
		return;
	
	default:
		assert(false);
	}
}

uint_addr_t MachineState::getInsnPtr() const
{
	return getRegister(X86_REG_RIP).tryGetKnownConst()->value;
}

void MachineState::setInsnPtr(uint_addr_t val)
{
	setRegister(X86_REG_RIP, SemanticKnownConst(val, sizeof(uint_addr_t), true)); //Ensure RIP is up to date
}

void MachineState::stackPush(SemanticValue value)
{
	assert(value.getSize() > 0);
	SemanticKnownConst rsp = *getRegister(X86_REG_RSP).tryGetKnownConst();
	rsp.value -= value.getSize(); //Make space on stack
	setMemory(rsp, value, value.getSize()); //Write to stack
	setRegister(X86_REG_RSP, rsp);
}

SemanticValue MachineState::stackPop(size_t nBytes)
{
	SemanticKnownConst rsp = *getRegister(X86_REG_RSP).tryGetKnownConst();
	SemanticValue out = getMemory(rsp, nBytes); //Read from stack
	rsp.value += nBytes; //Reclaim space on stack
	setRegister(X86_REG_RSP, rsp);
	assert(out.getSize() == nBytes);
	return out;
}

void MachineState::pushStackFrame(SemanticKnownConst fp)
{
	//Requires target address to be a known const. Will crash otherwise
	SemanticKnownConst rsp = *getRegister(X86_REG_RSP).tryGetKnownConst();
	SemanticValue      rbp =  getRegister(X86_REG_RBP);
	SemanticKnownConst rip = *getRegister(X86_REG_RIP).tryGetKnownConst();
		
	stackPush(rip); //Push return address to stack
	stackPush(rbp); //Push previous RBP to stack

	//Mark new stack frame location
	rbp = rsp;

	//Jump to new function
	rip = fp;

	//Write back to registers
	setRegister(X86_REG_RBP, rbp);
	setRegister(X86_REG_RIP, rip);
}

SemanticValue MachineState::popStackFrame()
{
	//Cannot pop to an indeterminate address
	if (getRegister(X86_REG_RBP).tryGetKnownConst())
	{
		//Pop previous stack frame (return address and RBP) from stack
		SemanticKnownConst rbp = *getRegister(X86_REG_RBP).tryGetKnownConst();
		SemanticKnownConst rip = *getRegister(X86_REG_RIP).tryGetKnownConst();
		SemanticValue oldRbp     = stackPop(rbp.size);
		SemanticValue returnAddr = stackPop(rip.size);

		setRegister(X86_REG_RBP, oldRbp); //Restore previous stack frame
		setRegister(X86_REG_RIP, returnAddr); //Jump to return address

		return returnAddr;
	}
	else
	{
		//Invalidate state
		size_t ripSize = getRegister(X86_REG_RIP).getSize();
		size_t rbpSize = getRegister(X86_REG_RBP).getSize();
		setRegister(X86_REG_RIP, SemanticUnknown(ripSize) );
		setRegister(X86_REG_RBP, SemanticUnknown(rbpSize) );

		//Pop from stack
		stackPop(ripSize + rbpSize);
		
		return SemanticUnknown(ripSize);
	}
}

std::optional<bool> MachineState::isConditionMet(unsigned int insnId) const
{
	//See https://www.felixcloutier.com/x86/jcc
	SemanticFlags flags = *getRegister(x86_reg::X86_REG_EFLAGS).tryGetFlags();
	switch (insnId)
	{
		case x86_insn::X86_INS_JMP: return true;

		case x86_insn::X86_INS_JO : return flags.check((int)MachineState::FlagIDs::Overflow, true );
		case x86_insn::X86_INS_JNO: return flags.check((int)MachineState::FlagIDs::Overflow, false);
		case x86_insn::X86_INS_JB : return flags.check((int)MachineState::FlagIDs::Carry   , true );
		case x86_insn::X86_INS_JAE: return flags.check((int)MachineState::FlagIDs::Carry   , false);
		case x86_insn::X86_INS_JE : return flags.check((int)MachineState::FlagIDs::Zero    , true );
		case x86_insn::X86_INS_JNE: return flags.check((int)MachineState::FlagIDs::Zero    , false);
		case x86_insn::X86_INS_JS : return flags.check((int)MachineState::FlagIDs::Sign    , true );
		case x86_insn::X86_INS_JNS: return flags.check((int)MachineState::FlagIDs::Sign    , false);
		case x86_insn::X86_INS_JP : return flags.check((int)MachineState::FlagIDs::Parity  , true );
		case x86_insn::X86_INS_JNP: return flags.check((int)MachineState::FlagIDs::Parity  , false);

		case x86_insn::X86_INS_JCXZ:
		case x86_insn::X86_INS_JECXZ:
		case x86_insn::X86_INS_JRCXZ: return getRegister(x86_reg::X86_REG_RCX).isZero();

		case x86_insn::X86_INS_JBE:
		{
			auto cf = flags.check((int)MachineState::FlagIDs::Carry, true);
			auto zf = flags.check((int)MachineState::FlagIDs::Zero, true);
			if (cf.has_value() && zf.has_value()) return cf.value() || zf.value(); //Determinate case
			else                                  return std::nullopt; //Indeterminate case
		}

		case x86_insn::X86_INS_JA:
		{
			auto cf = flags.check((int)MachineState::FlagIDs::Carry, true);
			auto zf = flags.check((int)MachineState::FlagIDs::Zero, true);
			if (cf.has_value() && zf.has_value()) return !cf.value() && !zf.value(); //Determinate case
			else                                  return std::nullopt; //Indeterminate case
			break;
		}

		case x86_insn::X86_INS_JL:
		{
			auto sf = flags.check((int)MachineState::FlagIDs::Sign, true);
			auto of = flags.check((int)MachineState::FlagIDs::Overflow, true);
			if (sf.has_value() && of.has_value()) return sf.value() != of.value(); //Determinate case
			else                                  return std::nullopt; //Indeterminate case
			break;
		}

		case x86_insn::X86_INS_JGE:
		{
			auto sf = flags.check((int)MachineState::FlagIDs::Sign, true);
			auto of = flags.check((int)MachineState::FlagIDs::Overflow, true);
			if (sf.has_value() && of.has_value()) return sf.value() == of.value(); //Determinate case
			else                                  return std::nullopt; //Indeterminate case
			break;
		}

		case x86_insn::X86_INS_JLE:
		{
			auto zf = flags.check((int)MachineState::FlagIDs::Zero, true);
			auto sf = flags.check((int)MachineState::FlagIDs::Sign, true);
			auto of = flags.check((int)MachineState::FlagIDs::Overflow, true);
			if (zf.has_value() && zf.value()) return true;
			else if (sf.has_value() && of.has_value()) return sf.value() != of.value(); //Determinate case
			else                                       return std::nullopt; //Indeterminate case
			break;
		}

		case x86_insn::X86_INS_JG:
		{
			auto zf = flags.check((int)MachineState::FlagIDs::Zero, true);
			auto sf = flags.check((int)MachineState::FlagIDs::Sign, true);
			auto of = flags.check((int)MachineState::FlagIDs::Overflow, true);
			if (zf.has_value() && sf.has_value() && of.has_value()) return !zf.value() && (sf.value() == of.value()); //Determinate case
			else                                                    return std::nullopt; //Indeterminate case
			break;
		}

		default:
			assert(false && "Unknown branch condition");
			return std::nullopt;
	}
}

/*
size_t MachineState::countStackFrames() const
{
	size_t n = 1;
	unwindStack([&](const SemanticValue& stackHighEndAddr, const SemanticValue& returnLocation) { ++n; });
	return n;
}

void MachineState::unwindStack(const StackVisitor& visitor) const
{
	SemanticValue rbp = getMemory(*registers[X86_REG_RBP], sizeof(void*));
	while (!rbp.isUnknown())
	{
		SemanticKnownConst offset(sizeof(void*), sizeof(void*));
		SemanticValue oldRbp     = getMemory(rbp       , sizeof(void*)); //Read from stack
		SemanticValue returnAddr = getMemory(rbp+offset, sizeof(void*)); //Read from stack

		visitor(rbp, returnAddr);
		
		rbp = oldRbp; //Pop rbp
	}
}
// */

int MachineState::debugPrintWorkingSet() const
{
	int bytesWritten = 0;
	//bytesWritten += printf("rcx=");
	//bytesWritten += getRegister(X86_REG_RCX).debugPrintValue();
	//bytesWritten += printf(" | rax=");
	//bytesWritten += getRegister(X86_REG_RAX).debugPrintValue();
	bytesWritten += printf(" | rsp=");
	bytesWritten += getRegister(X86_REG_RSP).debugPrintValue();
	//bytesWritten += printf(" | rbp=");
	//bytesWritten += getRegister(X86_REG_RBP).debugPrintValue();
	return bytesWritten;
}

const char* MachineState::checkGood() const
{
	if (!getRegister(X86_REG_RSP).tryGetKnownConst()) return "Lost track of stack pointer!";
	if (!getRegister(X86_REG_RIP).tryGetKnownConst()) return "Lost track of instruction pointer!";
	return nullptr;
}

void MachineState::requireGood() const
{
	if (const char* err = checkGood(); err)
	{
		printf("\n\n%s! This should never happen!\n", err);
		assert(false);
	}
}

SemanticValue mergeValues(const SemanticValue& a, const SemanticValue& b)
{
	SemanticUnknown unknown(std::max(a.getSize(), b.getSize()));
	if (a.isUnknown() || b.isUnknown()) return unknown;

	const auto* aConst = a.tryGetKnownConst();
	const auto* bConst = b.tryGetKnownConst();
	const auto* aMagic = a.tryGetMagic();
	const auto* bMagic = b.tryGetMagic();
	const auto* aFlags = a.tryGetFlags();
	const auto* bFlags = b.tryGetFlags();

	//If all flags are known, treat as const
	if (aFlags && aFlags->bitsKnown == ~0ull) aConst = (const SemanticKnownConst*) aFlags; //Flags' bits field corresponds to const's value field
	if (bFlags && bFlags->bitsKnown == ~0ull) bConst = (const SemanticKnownConst*) bFlags;

	if (aConst && bConst && aConst->bound() == bConst->bound()) return a;
	else if (aMagic && bMagic && aMagic->offset == bMagic->offset && aMagic->id == bMagic->id) return a;
	else if (aFlags && bFlags)
	{
		SemanticFlags flags;
		for (int i = 0; i < sizeof(flags.bits); ++i)
		{
			std::optional<bool> av = aFlags->check(i, true);
			std::optional<bool> bv = bFlags->check(i, true);
			if (av.has_value() && bv.has_value() && av.value() == bv.value()) flags.set(i, av.value());
			else flags.set(i, std::nullopt);
		}
		return flags;
	}
	else return unknown;
}

template<typename K, typename V>
void mergeMaps(std::map<K, V>& canonical, const std::map<K, V>& reference)
{
	//Remove elements in canonical not present in reference
	{
		auto it = std::find_if(canonical.begin(), canonical.end(),
			[&](const auto& i) {
				return reference.find(i.first) == reference.end();
			}
		);
		if (it != canonical.end()) canonical.erase(it);
	}

	//Merge by value
	for (const auto& refKv : reference)
	{
		auto canonKv = canonical.find(refKv.first);
		if (canonKv != canonical.end())
		{
			SemanticValue val = mergeValues(canonKv->second, refKv.second);
			if (!val.isUnknown()) canonical.at(refKv.first) = val;
			else canonical.erase(canonKv);
		}
	}
}

MachineState MachineState::merge(const std::vector<const MachineState*>& divergentStates)
{
	assert(divergentStates.size() > 0);
	if (divergentStates.size() == 1) return *divergentStates[0];

	MachineState canonical = *divergentStates[0];
	for (int stateID = 1; stateID < divergentStates.size(); ++stateID)
	{
		const MachineState* state = divergentStates[stateID];
		mergeMaps(canonical.__registerStorage , state->__registerStorage );
		mergeMaps(canonical.constMemory.memory, state->constMemory.memory);
		for (auto kv : state->magics)
		{
			if (!canonical.magics.count(kv.first)) canonical.magics.emplace(kv); //Trivial case: value not present, just copy memory space
			else mergeMaps(canonical.magics.at(kv.first).memory, kv.second.memory); //Complex case: present in both, attempt merge
		}
	}
	return canonical;
}
