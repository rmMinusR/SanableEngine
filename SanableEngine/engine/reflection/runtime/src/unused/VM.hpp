#pragma once

#include <variant>
#include <vector>

#include "TypeInfo.hpp"
#include "GenericFunction.hpp"

struct SemanticKnownConst //A continuous block of memory of known value
{
private:
	SemanticKnownConst();

public:
	uint8_t* value;
	size_t size;
	bool ownsValue;
	
	template<typename T>
	static SemanticKnownConst make(const T& val)
	{
		SemanticKnownConst out;
		out.size = sizeof(val);
		out.value = malloc(out.size);
		memcpy(out.value, &val, out.size)
		out.ownsValue = true;
		return out;
	}

	SemanticKnownConst&& slice(size_t start, size_t end) const;

	~SemanticKnownConst();
	SemanticKnownConst(const SemanticKnownConst& cpy);
	SemanticKnownConst& operator=(const SemanticKnownConst& cpy);
	SemanticKnownConst(SemanticKnownConst&& mov);
	SemanticKnownConst& operator=(SemanticKnownConst&& mov);
};
struct SemanticUnknown {};
struct SemanticThisPtr {};
using VMValue = std::variant<SemanticUnknown, SemanticKnownConst, SemanticThisPtr>;


class VM
{
	void simulateCallInternal(const GenericFunction& fn, const std::vector<VMValue>& args, const TypeInfo& ret, const std::vector<TypeInfo>& argTypes);
	
	VM();
public:
	~VM();

	VM(const VM& cpy) = delete;
	VM& operator=(const VM& cpy) = delete;
	VM(VM&& mov) = delete;
	VM& operator=(VM&& mov) = delete;
	
	template<typename TRet, typename... TArgs>
	void simulateCall(TRet(*fn)(TArgs...), const std::vector<VMValue>& args)
	{
		assert(args.size() == sizeof...(TArgs));

		std::vector<TypeInfo> argTypes;
		FunctionUtil::identifyArgs<0, TArgs...>(argTypes);
		
		simulateCallInternal(
			GenericFunction::captureThisCall(fn),
			args,
			TypeInfo::createDummy<TRet>(),
			argTypes
		);
	}
};
