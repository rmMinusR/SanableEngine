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
	/// <param name="fn">Function to simulate</param>
	/// <param name="expectedReturnAddress">Expected return address, or null if indeterminate/unchecked</param>
	/// <param name="indentLevel">For debugging</param>
	void execFunc_internal(void(*fn)(), void(*expectedReturnAddress)(), int indentLevel);

public:
	MachineState canonicalState;

	/// <summary>
	/// Simulate a single instruction
	/// </summary>
	/// <param name="insn">Instruction to simulate</param>
	/// <param name="pushCallStack">Callback for when a CALL happens</param>
	/// <param name="popCallStack">Callback for when a RET happens, which should yield the expected return address, or null if indeterminate/unchecked</param>
	void step(const cs_insn* insn, const std::function<void(void*)>& pushCallStack, const std::function<void*()>& popCallStack);

	/// <summary>
	/// Deterministically simulate an entire function (until its final RET is reached).
	/// You probably want to reset canonicalState before calling.
	/// </summary>
	/// <param name="fn">Function to simulate</param>
	void execFunc(void(*fn)());

	/// <param name="callConv">Calling convention</param>
	/// <param name="argTypes">Expected argument types</param>
	//void execFunc(void(*fn)(), CallConv callConv, const std::vector<TypeInfo>& argTypes, void(*expectedReturnAddress)());

	/// <summary>
	/// Evalute all given branches, and keep only the values shared across all.
	/// </summary>
	/// <param name="canonicalState">Starting state. Modified once complete to reflect shared values.</param>
	/// <param name="branchCursors">Expected argument types</param>
	static void execBranch(MachineState& canonicalState, const std::vector<void*>& branchCursors);
};
