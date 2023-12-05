#pragma once

#include <cstdint>

int debugPrintSignedHex(int64_t val);

struct SemanticUnknown /// A continuous span of unknown bytes, or the result of combination of mismatched types (spanning the largest size)
{
	size_t size = 0;
	SemanticUnknown(size_t size);
};
struct SemanticKnownConst /// A continuous span of known bytes. Combination with any other type will result in Unknown.
{
	size_t size;
	uint64_t value; //We don't need to support registers bigger than 64 bits right now

	SemanticKnownConst(uint64_t value, size_t size);
	uint8_t& byte(size_t index);
	uint64_t bound() const;
	SemanticKnownConst signExtend(size_t targetSizeBytes) const;
};
struct SemanticThisPtr /// Represents the "this" keyword plus some offset. Typically lives in eCX/rCX/CX.
{
	size_t size = sizeof(void*);
	size_t offset = 0;
	SemanticThisPtr(size_t offset);
};

struct SemanticValue
{
public:
	enum class Type
	{
		Unknown = 0, /// An value that hasn't been set, was combined with something else unknown, or was sheared
		KnownConst, /// A known constant value
		ThisPtr /// A magic value plus some offset
	};

private:
	union
	{
		size_t size;
		SemanticUnknown    asUnknown;
		SemanticKnownConst asKnownConst;
		SemanticThisPtr    asThisPtr;
	};
	Type valueType;

public:
	Type getType() const;
	size_t getSize() const;
	void resize(size_t); //Usually a very bad idea. But necessary for some things.
	bool isUnknown() const;
	SemanticKnownConst* tryGetKnownConst();
	SemanticThisPtr   * tryGetThisPtr   ();
	const SemanticKnownConst* tryGetKnownConst() const;
	const SemanticThisPtr   * tryGetThisPtr   () const;

	SemanticValue();
	SemanticValue(const SemanticUnknown&);
	SemanticValue(const SemanticKnownConst&);
	SemanticValue(const SemanticThisPtr&);
	void operator=(const SemanticUnknown&    val);
	void operator=(const SemanticKnownConst& val);
	void operator=(const SemanticThisPtr&    val);

	int debugPrintValue() const; //Returns number of chars printed
};

SemanticValue operator+(const SemanticValue& lhs, const SemanticValue& rhs);
SemanticValue operator-(const SemanticValue& lhs, const SemanticValue& rhs);
SemanticValue operator*(const SemanticValue& lhs, const SemanticValue& rhs);
inline SemanticValue& operator+=(SemanticValue& lhs, const SemanticValue& rhs) { lhs = lhs + rhs; return lhs; }
inline SemanticValue& operator-=(SemanticValue& lhs, const SemanticValue& rhs) { lhs = lhs - rhs; return lhs; }
inline SemanticValue& operator*=(SemanticValue& lhs, const SemanticValue& rhs) { lhs = lhs * rhs; return lhs; }
