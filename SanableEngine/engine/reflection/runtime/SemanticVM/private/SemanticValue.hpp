#pragma once

#include <cstdint>
#include <optional>
#include <algorithm>

int debugPrintSignedHex(int64_t val);

#define _XM_FOREACH_SEMANTICVALUE_TYPE_EXCEPT_UNKNOWN() \
	_X(KnownConst) \
	_X(Magic) \
	_X(Flags)
#define _XM_FOREACH_SEMANTICVALUE_TYPE() \
	_X(Unknown) _XM_FOREACH_SEMANTICVALUE_TYPE_EXCEPT_UNKNOWN()

struct SemanticUnknown /// A continuous span of unknown bytes, or the result of combination of mismatched types (spanning the largest size)
{
	size_t size = 0;
	SemanticUnknown(size_t size);
};
struct SemanticKnownConst /// A continuous span of known bytes. Combination with any other type will result in Unknown.
{
	size_t size;
	uint64_t value; //We don't need to support registers bigger than 64 bits right now
	bool isPositionIndependentAddr;

	SemanticKnownConst(uint64_t value, size_t size, bool isPositionIndependentAddr);
	uint8_t& byte(size_t index);
	uint64_t bound() const;
	uint64_t mask() const; //Also equivalent to unsigned max or signed -1
	SemanticKnownConst signExtend(size_t targetSizeBytes) const;
	bool isSigned() const;
	void setSign(bool sign);
};
struct SemanticMagic /// Represents a magic value (such as heap allocations, or the "this" pointer) plus some offset. Typically lives in ECX for __thiscall, or returned in EAX by malloc.
{
	size_t size;
	size_t offset;
	typedef int64_t id_t;
	id_t id;
	SemanticMagic(size_t size, size_t offset, id_t id);
};
struct SemanticFlags /// Represents a bitfield of at most 64 bits, each of which can be in a known or unknown state
{
	size_t size = sizeof(uint64_t); //Do not modify
	uint64_t bits = 0;
	uint64_t bitsKnown = 0;
	SemanticFlags();
	SemanticFlags(uint64_t bits, uint64_t known);
	std::optional<bool> check(int id, bool expectedValue) const;
	void set(int id, std::optional<bool> val);
	bool allKnown(const std::initializer_list<int>& ids) const;
};

#define _X(id) static_assert(sizeof(Semantic##id) != 0); //Ensure each specified type exists
_XM_FOREACH_SEMANTICVALUE_TYPE()
#undef _X

struct SemanticValue
{
public:

	enum class Type
	{
#define _X(id) id,
		_XM_FOREACH_SEMANTICVALUE_TYPE()
#undef _X
	};

private:
	Type valueType;
	union
	{
		size_t size;
#define _X(id) Semantic##id as##id;
		_XM_FOREACH_SEMANTICVALUE_TYPE()
#undef _X
	};

#define _X(id) sizeof(Semantic##id),
	constexpr static size_t variantSize = std::max({ _XM_FOREACH_SEMANTICVALUE_TYPE() (size_t)0 });
#undef _X

public:
	Type getType() const;
	size_t getSize() const;
	void resize(size_t); //Usually a very bad idea. But necessary for some things.
	bool isUnknown() const;
	std::optional<bool> isZero() const;

	//Getters
#define _X(id) \
	Semantic##id* tryGet##id(); \
	const Semantic##id* tryGet##id() const;

	_XM_FOREACH_SEMANTICVALUE_TYPE_EXCEPT_UNKNOWN()
#undef _X

	SemanticValue();

	//Converting constructors and operator=
#define _X(id) \
	SemanticValue(const Semantic##id&); \
	SemanticValue& operator=(const Semantic##id& val);

	_XM_FOREACH_SEMANTICVALUE_TYPE()
#undef _X

	bool operator==(const SemanticValue& rhs) const;
	bool operator!=(const SemanticValue& rhs) const;

	int debugPrintValue(bool pad = true) const; //Returns number of chars printed
};

SemanticValue operator+(const SemanticValue& lhs, const SemanticValue& rhs);
SemanticValue operator-(const SemanticValue& lhs, const SemanticValue& rhs);
SemanticValue operator*(const SemanticValue& lhs, const SemanticValue& rhs);
SemanticValue operator&(const SemanticValue& lhs, const SemanticValue& rhs);
SemanticValue operator|(const SemanticValue& lhs, const SemanticValue& rhs);
SemanticValue operator^(const SemanticValue& lhs, const SemanticValue& rhs);
inline SemanticValue& operator+=(SemanticValue& lhs, const SemanticValue& rhs) { lhs = lhs + rhs; return lhs; }
inline SemanticValue& operator-=(SemanticValue& lhs, const SemanticValue& rhs) { lhs = lhs - rhs; return lhs; }
inline SemanticValue& operator*=(SemanticValue& lhs, const SemanticValue& rhs) { lhs = lhs * rhs; return lhs; }
inline SemanticValue& operator&=(SemanticValue& lhs, const SemanticValue& rhs) { lhs = lhs & rhs; return lhs; }
inline SemanticValue& operator|=(SemanticValue& lhs, const SemanticValue& rhs) { lhs = lhs | rhs; return lhs; }
inline SemanticValue& operator^=(SemanticValue& lhs, const SemanticValue& rhs) { lhs = lhs ^ rhs; return lhs; }
