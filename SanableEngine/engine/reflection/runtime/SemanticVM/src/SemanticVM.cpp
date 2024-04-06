#include "SemanticVM.hpp"

#include <cassert>

bool SemanticVM::debug = false;

void SemanticVM::step(MachineState& state, const cs_insn* insn, const std::function<void(void*)>& pushCallStack, const std::function<void*()>& popCallStack, const std::function<void(void*)>& jump, const std::function<void(const std::vector<void*>&)>& fork)
{
	bool handled = 
		   step_dataflow(state, insn)
		|| step_cmpmath(state, insn)
		|| step_math(state, insn)
		|| step_bitmath(state, insn)
		|| step_execflow(state, insn, pushCallStack, popCallStack, jump, fork);

	if (!handled)
	{
		if (platform_isInterrupt(*insn)) assert(false && "Caught an interrupt! Can't continue emulated execution.");
		else
		{
			printf("WARNING: Unknown operation\n");
			step_invalidate(state, insn);
		}
	}
}

//Helpers for execFunc and its siblings
struct FunctionContext
{
	struct branch_t
	{
		MachineState state;
		bool keepExecuting;

		//Stuff Capstone wants
		const uint8_t* cursor;
		uint64_t addr;

		void parseNext(cs_insn* insn_out)
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
	};

	std::vector<branch_t> branches;
	cs_insn* insn;
	SemanticVM::ExecutionOptions opt;

	FunctionContext(MachineState initialState, void(*fn)(), const SemanticVM::ExecutionOptions& opt) :
		opt(opt)
	{
		pushBranch(initialState, fn);
		insn = cs_malloc(capstone_get_instance());
	}

	~FunctionContext()
	{
		cs_free(insn, 1);
	}

	void pushBranch(MachineState& state, void(*fn)())
	{
		branches.push_back({ state, true });
		branches[0].state.setInsnPtr((uint_addr_t)fn);
	}
	void canonizeCoincidentBranches(bool debug)
	{
		//Canonize branches with the same cursor
		for (int i = 0; i < branches.size(); ++i)
		{
			for (int j = i+1; j < branches.size(); ++j)
			{
				if (branches[i].state.getInsnPtr() == branches[j].state.getInsnPtr() && branches[i].keepExecuting && branches[j].keepExecuting)
				{
					if (debug) printf("Branch #%i merged into #%i\n", j, i);
					branches[i].state = MachineState::merge({ &branches[i].state, &branches[j].state }); //Canonize state
					branches.erase(branches.begin()+i); //Erase copy
					j--; //Don't skip!
				}
			}
		}
	}
	int getEarliestBranch() const
	{
		//Execute the cursor that's furthest behind
		int toExecIndex = -1;
		for (int i = 0; i < branches.size(); ++i)
		{
			if (branches[i].keepExecuting && (toExecIndex == -1 || branches[i].state.getInsnPtr() < branches[toExecIndex].state.getInsnPtr()))
			{
				toExecIndex = i;
			}
		}
		return toExecIndex;
	}
	
	//Returns true if handled, false if standard behavior
	bool callSpecial(int srcBranchID, void(*targetFn)(), bool debug)
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
		else if (std::find(opt.sandboxed.begin(), opt.sandboxed.end(), targetFn) != opt.sandboxed.end())
		{
			//TODO implement
			src.state.popStackFrame(); //TEMP: Undo pushing stack frame
			if (debug) printf("Function is sandboxed, and will not be simulated.\n");
			return true;
		}
		else return false;
	}

private:
	SemanticMagic::id_t nextMagicID = 0;
public:
	SemanticMagic::id_t requestMagicID() { return nextMagicID++; }
	SemanticMagic requestAllocation() { return SemanticMagic(sizeof(void*), 0, requestMagicID()); }
};

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
			[&](void* fn) { callTarget = (void(*)()) fn; }, //On CALL
			[&]() { //On RET
				EXEC.keepExecuting = false;
				if (debug) printf("   ; execution terminated", toExecIndex, EXEC.cursor);
				return expectedReturnAddress;
			},
			[&](void* jmp) { jmpTargets = { jmp }; }, //On jump
			[&](const std::vector<void*>& forks) { jmpTargets = forks; } //On fork
		);
		
		//Handle jumping/branching
		if (jmpTargets.size() == 1) EXEC.state.setInsnPtr((uint_addr_t)jmpTargets[0]); //Determinate case: jump
		else if (jmpTargets.size() > 1) //Indeterminate case: branch
		{
			for (void* i : jmpTargets) assert(i >= EXEC.cursor && "Indeterminate looping not supported");

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
		if (EXEC.keepExecuting) EXEC.state.requireGood();

		//If we're supposed to call another function, do so
		if (callTarget)
		{
			bool special = context.callSpecial(toExecIndex, callTarget, debug);
			if (!special) execFunc_internal(EXEC.state, callTarget, (void(*)())EXEC.cursor, indentLevel+1, opt);
		}

		#undef EXEC
	}

	//Canonize all remaining states
	std::vector<const MachineState*> divergentStates;
	for (const auto& branch : context.branches) divergentStates.push_back(&branch.state);
	state = MachineState::merge(divergentStates); //TODO handle sandboxing
}
