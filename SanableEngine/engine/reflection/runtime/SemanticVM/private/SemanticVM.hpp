#pragma once

#include <functional>

#include "MachineState.hpp"
#include "GenericFunction.hpp"

class SemanticVM
{
	/// <summary>
	/// Simulate an entire function WITHOUT any preamble, or memory/register setup.
	/// If branches occur, only shared known constants/magics will be considered canonical.
	/// </summary>
	/// <param name="state">Starting state. Modified once complete to reflect altered values.</param>
	/// <param name="fn">Function to simulate</param>
	/// <param name="expectedReturnAddress">Expected return address, or null if indeterminate/unchecked</param>
	/// <param name="indentLevel">For debugging</param>
	static void execFunc_internal(MachineState& state, void(*fn)(), void(*expectedReturnAddress)(), int indentLevel);

public:
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
					 const std::function<void(void*)>& pushCallStack,
					 const std::function<void*()>& popCallStack,
					 const std::function<void(void*)>& jump,
					 const std::function<void(const std::vector<void*>&)>& fork);

	/// <summary>
	/// Deterministically simulate an entire function (until its final RET is reached).
	/// You probably want to reset canonicalState before calling.
	/// </summary>
	/// <param name="state">Starting state. Modified once complete to reflect altered values.</param>
	/// <param name="fn">Function to simulate. NOTE: Arguments and return values are currently not supported.</param>
	static void execFunc(MachineState& state, void(*fn)());

	/// <summary>
	/// Evalute all given branches, and keep only the values shared across all.
	/// </summary>
	/// <param name="canonicalState">Starting state. Modified once complete to reflect shared values.</param>
	/// <param name="branchCursors">Expected argument types</param>
	static void execBranch(MachineState& canonicalState, const std::vector<void*>& branchCursors);
};
