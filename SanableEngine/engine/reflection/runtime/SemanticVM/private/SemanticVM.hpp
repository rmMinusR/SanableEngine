#pragma once

#include <functional>
#include <string>

#include "MachineState.hpp"

struct FunctionContext;

class SemanticVM
{
public:
	friend struct FunctionContext;

	struct ExecutionOptions
	{
		bool canReadHostMemory = false;
		std::vector<void(*)()> allocators; /// Relevant memory-allocating functions, such as malloc or operator new
		std::vector<void(*)()> sandboxed; /// Functions not allowed to write to memory, such as memset during vtable detection. They will still be able to modify registers/stack pointers.
		bool isSandboxAllowList = false; /// If true, inverts the behavior of sandboxed.
		bool continueOnError = false;
		bool executeSubFunctions = true;

		inline bool canExec(void(*fn)())
		{
			bool existsInSandboxed = (std::find(sandboxed.begin(), sandboxed.end(), fn) != sandboxed.end());
			return existsInSandboxed == isSandboxAllowList;
		}
	};

private:
	/// <summary>
	/// Simulate an entire function WITHOUT any preamble, or memory/register setup.
	/// If branches occur, only shared known constants/magics will be considered canonical.
	/// </summary>
	/// <param name="state">Starting state. Modified once complete to reflect altered values.</param>
	/// <param name="fn">Function to simulate</param>
	/// <param name="expectedReturnAddress">Expected return address, or null if indeterminate/unchecked</param>
	/// <param name="indentLevel">For debugging</param>
	/// <param name="opt">Additional options for execution</param>
	static void execFunc_internal(MachineState& state, void(*fn)(), void(*expectedReturnAddress)(), int indentLevel, const ExecutionOptions& opt);
	
public:
	static bool debug; //Default false. Set true to enable debugging.

	/// <summary>
	/// Simulate a single instruction
	/// </summary>
	/// <param name="state">State of the given machine. Instruction pointer should be updated manually prior to calling this function.</param>
	/// <param name="pushCallStack">Callback for when a CALL happens</param>
	/// <param name="popCallStack">Callback for when a RET happens, which should yield the expected return address, or null if indeterminate/unchecked</param>
	/// <param name="jump">Callback for when a jump or determinate branch occurs.</param>
	/// <param name="fork">Callback for when an indeterminate branch occurs. If branching is determinate, jump callback will be used instead.</param>
	static void step(MachineState& state,
			const cs_insn* insn,
			const std::function<void(const std::string&)>& reportError,
			const std::function<void(void*)>& pushCallStack,
			const std::function<void*()>& popCallStack,
			const std::function<void(void*)>& jump,
			const std::function<void(const std::vector<void*>&)>& fork);
	
	//Per-instruction-group step functions: return true if handled
	static bool step_dataflow(MachineState& state, const cs_insn* insn, const std::function<void(const std::string&)>& reportError); //Data flow: MOV, LEA, PUSH, etc
	static bool step_cmpmath (MachineState& state, const cs_insn* insn, const std::function<void(const std::string&)>& reportError); //Math that might be relevant to branching: CMP, TEST
	static bool step_math    (MachineState& state, const cs_insn* insn, const std::function<void(const std::string&)>& reportError); //Arithmetic: ADD, ADC, SUB, MUL
	static bool step_bitmath (MachineState& state, const cs_insn* insn, const std::function<void(const std::string&)>& reportError); //Bit twiddling: ROR, AND, XOR
	static bool step_execflow(MachineState& state, const cs_insn* insn, const std::function<void(const std::string&)>& reportError,
		const std::function<void(void*)>& pushCallStack,
		const std::function<void* ()>& popCallStack,
		const std::function<void(void*)>& jump,
		const std::function<void(const std::vector<void*>&)>& fork); //Flow of execution: JMP, NOP, CALL, RET, but not INT
	static void step_invalidate(MachineState& state, const cs_insn* insn); //Quick utility to invalidate all register state from the given instruction


	/// <summary>
	/// Deterministically simulate an entire function (until its final RET is reached).
	/// You probably want to reset canonicalState before calling.
	/// </summary>
	/// <param name="state">Starting state. Modified once complete to reflect altered values.</param>
	/// <param name="fn">Function to simulate. NOTE: Arguments and return values are currently not supported.</param>
	/// <param name="opt">Additional options for execution</param>
	static void execFunc(MachineState& state, void(*fn)(), const ExecutionOptions& opt);
};


//Helpers for execFunc and its siblings
struct FunctionContext
{
	struct branch_t
	{
		MachineState state;
		enum class ExecutionStatus
		{
			Executing,
			Returned,
			Errored
		} executionStatus;

		branch_t(const MachineState& src, void(*cursor)());

		//Stuff Capstone wants
		const uint8_t* cursor;
		uint64_t addr;

		void parseNext(cs_insn* insn_out);
		bool isExecuting() const;
	};

	std::vector<branch_t> branches;
	cs_insn* insn;
	SemanticVM::ExecutionOptions opt;

	FunctionContext(const MachineState& initialState, void(*fn)(), const SemanticVM::ExecutionOptions& opt);
	~FunctionContext();

	void pushBranch(const MachineState& state, void(*fn)());
	void canonizeCoincidentBranches(bool debug);
	int getEarliestBranch() const;
	
	//Returns true if handled, false if standard behavior
	bool callSpecial(int srcBranchID, void(*targetFn)(), bool debug, int currentIndentLevel);

private:
	SemanticMagic::id_t nextMagicID = 0;
public:
	SemanticMagic::id_t requestMagicID();
	SemanticMagic requestAllocation();
};
