#include "MachineState.hpp"

template<typename T1, typename T2, typename T3, typename T4>
GeneralValue GeneralValue_doMathOp(GeneralValue arg1, GeneralValue arg2,
	T1 funcConstConst, T2 funcConstThis, T3 funcThisConst, T4 funcThisThis)
{
		 if (std::holds_alternative<SemanticUnknown   >(arg1) || std::holds_alternative<SemanticUnknown   >(arg2)) return SemanticUnknown();
	else if (std::holds_alternative<SemanticKnownConst>(arg1) && std::holds_alternative<SemanticKnownConst>(arg2))
	{
		auto a = std::get<SemanticKnownConst>(arg1);
		auto b = std::get<SemanticKnownConst>(arg2);
		assert(a.size == b.size);
		return funcConstConst(a, b);
	}
	else if (std::holds_alternative<SemanticKnownConst>(arg1) && std::holds_alternative<SemanticThisPtr   >(arg2)) return funcConstThis (std::get<SemanticKnownConst>(arg1), std::get<SemanticThisPtr   >(arg2));
	else if (std::holds_alternative<SemanticThisPtr   >(arg1) && std::holds_alternative<SemanticKnownConst>(arg2)) return funcThisConst (std::get<SemanticThisPtr   >(arg1), std::get<SemanticKnownConst>(arg2));
	else if (std::holds_alternative<SemanticThisPtr   >(arg1) && std::holds_alternative<SemanticThisPtr   >(arg2)) return funcThisThis  (std::get<SemanticThisPtr   >(arg1), std::get<SemanticThisPtr   >(arg2));
	else { assert(false); return GeneralValue(); }
}

GeneralValue operator+(const GeneralValue& lhs, const GeneralValue& rhs)
{
	return GeneralValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.value + arg2.value, arg1.size); },
		[](SemanticKnownConst arg1, SemanticThisPtr    arg2) { return SemanticThisPtr{ arg1.value + arg2.offset }; },
		[](SemanticThisPtr    arg1, SemanticKnownConst arg2) { return SemanticThisPtr{ arg1.offset + arg2.value }; },
		[](SemanticThisPtr    arg1, SemanticThisPtr    arg2) { return SemanticUnknown(); }
	);
}

GeneralValue operator-(const GeneralValue& lhs, const GeneralValue& rhs)
{
	return GeneralValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.value - arg2.value, arg1.size); },
		[](SemanticKnownConst arg1, SemanticThisPtr    arg2) { return SemanticUnknown(); },
		[](SemanticThisPtr    arg1, SemanticKnownConst arg2) { return SemanticThisPtr{ arg1.offset - arg2.value }; },
		[](SemanticThisPtr    arg1, SemanticThisPtr    arg2) { return SemanticKnownConst(arg1.offset - arg2.offset, sizeof(void*)); }
	);
}

GeneralValue operator*(const GeneralValue& lhs, const GeneralValue& rhs)
{
	return GeneralValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.value * arg2.value, arg1.size); },
		[](SemanticKnownConst arg1, SemanticThisPtr    arg2) { return SemanticUnknown(); },
		[](SemanticThisPtr    arg1, SemanticKnownConst arg2) { return SemanticUnknown(); },
		[](SemanticThisPtr    arg1, SemanticThisPtr    arg2) { return SemanticUnknown(); }
	);
}
