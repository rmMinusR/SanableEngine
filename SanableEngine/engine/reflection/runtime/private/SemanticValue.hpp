#pragma once

#include <cstdint>
#include <variant>

struct SemanticUnknown {};
struct SemanticKnownConst
{
	uint64_t value = 0; //We don't need to support registers bigger than 64 bits right now
	size_t size = 0;

	SemanticKnownConst(uint64_t v, size_t s);
	uint8_t& byte(size_t index);
};
struct SemanticThisPtr //Represents the "this" keyword plus some offset. Typically lives in eCX/rCX/CX.
{
	size_t offset = 0;
	inline SemanticThisPtr(size_t offset) : offset(offset) {}
};
using SemanticValue = std::variant<SemanticUnknown, SemanticKnownConst, SemanticThisPtr>;
SemanticValue operator+(const SemanticValue& lhs, const SemanticValue& rhs);
SemanticValue operator-(const SemanticValue& lhs, const SemanticValue& rhs);
SemanticValue operator*(const SemanticValue& lhs, const SemanticValue& rhs);
