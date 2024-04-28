#include "SemanticVM.hpp"

#include <cassert>

bool SemanticVM::debug = true;

void SemanticVM::step(MachineState& state, const cs_insn* insn, const std::function<void(const std::string&)>& reportError, const std::function<void(void*)>& pushCallStack, const std::function<void*()>& popCallStack, const std::function<void(void*)>& jump, const std::function<void(const std::vector<void*>&)>& fork)
{
	bool handled = 
		   step_dataflow(state, insn, reportError)
		|| step_cmpmath (state, insn, reportError)
		|| step_math    (state, insn, reportError)
		|| step_bitmath (state, insn, reportError)
		|| step_execflow(state, insn, reportError, pushCallStack, popCallStack, jump, fork);

	if (!handled)
	{
		if (platform_isInterrupt(*insn)) reportError("Caught an interrupt! Can't continue emulated execution.");
		else
		{
			printf("WARNING: Unknown operation\n");
			step_invalidate(state, insn);
		}
	}
}

FunctionContext::branch_t::branch_t(const MachineState& src, void(*fn)()) :
	state(src),
	executionStatus(ExecutionStatus::Executing)
{
	state.setInsnPtr((uint_addr_t)fn);
}

void FunctionContext::branch_t::parseNext(cs_insn* insn_out)
{
	//Stuff that Capstone wants in a rather jank format
	addr = state.getInsnPtr();
	cursor = (uint8_t*)addr;
	size_t allowedToProcess = sizeof(cs_insn::bytes); //No way to know for sure, but we can do some stuff with JUMP/RET detection to figure it out

	//Advance cursor and interpret next
	bool disassemblyGood = cs_disasm_iter(capstone_get_instance(), &cursor, &allowedToProcess, &addr, insn_out);
	assert(disassemblyGood && "An internal error occurred with the Capstone disassembler.");

	//Update instruction pointer from Capstone
	state.setInsnPtr(addr);
}

bool FunctionContext::branch_t::isExecuting() const
{
	return executionStatus == ExecutionStatus::Executing;
}

FunctionContext::FunctionContext(const MachineState& initialState, void(*fn)(), const SemanticVM::ExecutionOptions& opt) :
	opt(opt)
{
	pushBranch(initialState, fn);
	insn = cs_malloc(capstone_get_instance());
}

FunctionContext::~FunctionContext()
{
	cs_free(insn, 1);
}

void FunctionContext::pushBranch(const MachineState& state, void(*fn)())
{
	branches.emplace_back(state, fn);
}
void FunctionContext::canonizeCoincidentBranches(bool debug)
{
	//Canonize branches with the same cursor
	for (int i = 0; i < branches.size(); ++i)
	{
		for (int j = i+1; j < branches.size(); ++j)
		{
			if (branches[i].state.getInsnPtr() == branches[j].state.getInsnPtr() && branches[i].isExecuting() && branches[j].isExecuting())
			{
				if (debug) printf("Branch #%i merged into #%i\n", j, i);
				branches[i].state = MachineState::merge({ &branches[i].state, &branches[j].state }); //Canonize state
				branches.erase(branches.begin()+i); //Erase copy
				j--; //Don't skip!
			}
		}
	}
}
int FunctionContext::getEarliestBranch() const
	{
		//Execute the cursor that's furthest behind
		int toExecIndex = -1;
		for (int i = 0; i < branches.size(); ++i)
		{
			if (branches[i].isExecuting() && (toExecIndex == -1 || branches[i].state.getInsnPtr() < branches[toExecIndex].state.getInsnPtr()))
			{
				toExecIndex = i;
			}
		}
		return toExecIndex;
	}
bool FunctionContext::callSpecial(int srcBranchID, void(*targetFn)(), bool debug, int currentIndentLevel)
{
	branch_t& src = branches[srcBranchID];
	if (std::find(opt.allocators.begin(), opt.allocators.end(), targetFn) != opt.allocators.end())
	{
		//No need to call function, just mark that we're creating a new heap object

		//TODO proper register detection
		//std::vector<x86_reg> covariants = ???;
		//assert(covariants.size() == 1);
		src.state.setRegister(x86_reg::X86_REG_EAX, requestAllocation());
		src.state.popStackFrame(); //Undo pushing stack frame
		if (debug) printf("Allocated heap object #%i. Allocator function will not be simulated.\n", 0);
		return true;
	}
	else if (!opt.canExec(targetFn))
	{
		MachineState sandbox = src.state;
		SemanticVM::execFunc_internal(sandbox, targetFn, (void(*)())src.cursor, currentIndentLevel+1, opt);
		MachineState::copyCriticals(src.state, sandbox);
		if (debug) printf("Function is sandboxed. Only propagating stack and instruction pointer changes.\n");
		return true;
	}
	else return false;
}

SemanticMagic::id_t FunctionContext::requestMagicID() { return nextMagicID++; }
SemanticMagic FunctionContext::requestAllocation() { return SemanticMagic(sizeof(void*), 0, requestMagicID()); }

void SemanticVM::execFunc_internal(MachineState& state, void(*fn)(), void(*expectedReturnAddress)(), int indentLevel, const ExecutionOptions& opt)
{
	FunctionContext context(state, fn, opt);
	while (true)
	{
		context.canonizeCoincidentBranches(debug);

		//Execute the cursor that's furthest behind
		int toExecIndex = context.getEarliestBranch();
		if (toExecIndex == -1) break; //All branches have terminated

		//Run Capstone
		#define EXEC context.branches[toExecIndex]
		EXEC.parseNext(context.insn);

		std::function handleError = [&](const std::string& msg)
		{
			printf("\nERROR: %s\n", msg.c_str());
			if (opt.continueOnError) EXEC.executionStatus = FunctionContext::branch_t::ExecutionStatus::Errored;
			else assert(false);
		};

		//DEBUG
		if (debug)
		{
			EXEC.state.debugPrintWorkingSet();
			for (int i = 0; i < indentLevel; ++i) printf(" |  "); //Indent
			printf("[Branch %2i] ", toExecIndex);
			printInstructionCursor(context.insn);
		}

		//Execute current call frame, one opcode at a time
		void(*callTarget)() = nullptr;
		std::vector<void*> jmpTargets; //Empty if no JMP, 1 element if determinate JMP, 2+ elements if indeterminate JMP
		step(EXEC.state, context.insn,
			handleError,
			[&](void* fn) { callTarget = (void(*)()) fn; }, //On CALL
			[&]() { //On RET
				EXEC.executionStatus = FunctionContext::branch_t::ExecutionStatus::Returned;
				if (debug) printf("   ; execution terminated", toExecIndex, EXEC.cursor);
				return expectedReturnAddress;
			},
			[&](void* jmp) { jmpTargets = { jmp }; }, //On jump
			[&](const std::vector<void*>& forks) { //On fork
				jmpTargets = forks;
				for (int i = jmpTargets.size()-1; i >= 0; --i)
				{
					if (jmpTargets[i] <= EXEC.cursor)
					{
						handleError("Indeterminate looping not supported");
						jmpTargets.erase(jmpTargets.begin()+i);
					}
				}
			}
		);
		
		//Handle jumping/branching
		if (jmpTargets.size() == 1) EXEC.state.setInsnPtr((uint_addr_t)jmpTargets[0]); //Determinate case: jump
		else if (jmpTargets.size() > 1) //Indeterminate case: branch
		{
			if (debug) printf("   ; Spawned branches ", toExecIndex, EXEC.cursor);
			EXEC.state.setInsnPtr((uint_addr_t)jmpTargets[0]);
			for (int i = 1; i < jmpTargets.size(); ++i)
			{
				if (debug) printf("#%i@%x ", i, jmpTargets[i]);
				context.pushBranch(EXEC.state, (void(*)())jmpTargets[i]); //Can't reference exec here in case the backing block reallocates
			}
		}

		//DEBUG
		if (debug) printf("\n");
		if (EXEC.isExecuting()) EXEC.state.requireGood();

		//If we're supposed to call another function, do so
		if (callTarget)
		{
			if (opt.executeSubFunctions) //FIXME this won't catch idioms like push rip > jmp %func%
			{
				bool special = context.callSpecial(toExecIndex, callTarget, debug, indentLevel);
				if (!special) execFunc_internal(EXEC.state, callTarget, (void(*)())EXEC.cursor, indentLevel+1, opt);
			}
			else EXEC.state.popStackFrame(); //Undo CALL
		}

		#undef EXEC
	}

	//Canonize all remaining states
	std::vector<const MachineState*> divergentStates;
	for (const auto& branch : context.branches) if (branch.executionStatus == FunctionContext::branch_t::ExecutionStatus::Returned) divergentStates.push_back(&branch.state);
	assert(!divergentStates.empty());
	state = MachineState::merge(divergentStates); //TODO handle sandboxing
}
