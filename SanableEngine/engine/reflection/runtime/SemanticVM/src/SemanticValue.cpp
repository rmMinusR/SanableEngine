#include "SemanticValue.hpp"

#include <cassert>
#include <functional>


SemanticUnknown::SemanticUnknown(size_t size) :
	size(size)
{
}

SemanticKnownConst::SemanticKnownConst(uint64_t v, size_t s) :
	size(s),
	value(v)
{
}

uint8_t& SemanticKnownConst::byte(size_t index)
{
	assert(index < size);
	return reinterpret_cast<uint8_t*>(&value)[index];
}

uint64_t SemanticKnownConst::bound() const
{
	     if (size == 1) return (uint8_t )value;
	else if (size == 2) return (uint16_t)value;
	else if (size == 4) return (uint32_t)value;
	else if (size == 8) return (uint64_t)value;
	else
	{
		assert(false);
		return 0;
	}
}

SemanticKnownConst SemanticKnownConst::signExtend(size_t targetSizeBytes) const
{
	int64_t val = 0;
	     if (size == 1) val = (int8_t )value;
	else if (size == 2) val = (int16_t)value;
	else if (size == 4) val = (int32_t)value;
	else if (size == 8) val = (int64_t)value;
	else assert(false);
	return SemanticKnownConst(val, targetSizeBytes);
}

SemanticThisPtr::SemanticThisPtr(size_t offset) :
	offset(offset)
{
}


SemanticValue::Type SemanticValue::getType() const
{
	return valueType;
}

size_t SemanticValue::getSize() const
{
	return size;
}

void SemanticValue::resize(size_t newSize)
{
	size = newSize;
}

bool SemanticValue::isUnknown() const
{
	return valueType == Type::Unknown || getSize() == 0;
}

SemanticKnownConst* SemanticValue::tryGetKnownConst()
{
	if (valueType == Type::KnownConst) return &asKnownConst;
	else return nullptr;
}

SemanticThisPtr* SemanticValue::tryGetThisPtr()
{
	if (valueType == Type::ThisPtr) return &asThisPtr;
	else return nullptr;
}

const SemanticKnownConst* SemanticValue::tryGetKnownConst() const
{
	if (valueType == Type::KnownConst) return &asKnownConst;
	else return nullptr;
}

const SemanticThisPtr* SemanticValue::tryGetThisPtr() const
{
	if (valueType == Type::ThisPtr) return &asThisPtr;
	else return nullptr;
}


#pragma region Overhead (boring)

SemanticValue::SemanticValue()                              { *this = SemanticUnknown(0); }
SemanticValue::SemanticValue(const SemanticUnknown   & val) { *this = val; }
SemanticValue::SemanticValue(const SemanticKnownConst& val) { *this = val; }
SemanticValue::SemanticValue(const SemanticThisPtr   & val) { *this = val; }

void SemanticValue::operator=(const SemanticUnknown& val)
{
	valueType = Type::Unknown;
	asUnknown = val;
}

void SemanticValue::operator=(const SemanticKnownConst& val)
{
	valueType = Type::KnownConst;
	asKnownConst = val;
}

void SemanticValue::operator=(const SemanticThisPtr& val)
{
	valueType = Type::ThisPtr;
	asThisPtr = val;
}

#pragma endregion


#pragma region Math

SemanticValue SemanticValue_doMathOp(SemanticValue arg1, SemanticValue arg2,
	const std::function<SemanticValue(SemanticKnownConst, SemanticKnownConst)>& funcConstConst, //Any of these can be null and we'll just return default unknown
	const std::function<SemanticValue(SemanticKnownConst, SemanticThisPtr   )>& funcConstThis,
	const std::function<SemanticValue(SemanticThisPtr   , SemanticKnownConst)>& funcThisConst,
	const std::function<SemanticValue(SemanticThisPtr   , SemanticThisPtr   )>& funcThisThis)
{
	auto* const1 = arg1.tryGetKnownConst();
	auto* const2 = arg2.tryGetKnownConst();
	auto* pThis1 = arg1.tryGetThisPtr();
	auto* pThis2 = arg2.tryGetThisPtr();

	SemanticUnknown unknown(std::max(arg1.getSize(), arg2.getSize()));
	if (arg1.isUnknown() || arg2.isUnknown()) return unknown;
	else if (const1 && const2)
	{
		assert(const1->size == const2->size);
		return funcConstConst ? funcConstConst(*const1, *const2) : unknown;
	}
	else if (const1 && pThis2) return funcConstThis ? funcConstThis(*const1, *pThis2) : unknown;
	else if (pThis1 && const2) return funcThisConst ? funcThisConst(*pThis1, *const2) : unknown;
	else if (pThis1 && pThis2) return funcThisThis  ? funcThisThis (*pThis1, *pThis2) : unknown;
	else { assert(false); return SemanticValue(); }
}

SemanticValue operator+(const SemanticValue& lhs, const SemanticValue& rhs)
{
	return SemanticValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.bound() + arg2.bound(), arg1.size); },
		[](SemanticKnownConst arg1, SemanticThisPtr    arg2) { return SemanticThisPtr{ arg1.bound() + arg2.offset}; },
		[](SemanticThisPtr    arg1, SemanticKnownConst arg2) { return SemanticThisPtr{ arg1.offset + arg2.bound() }; },
		nullptr //this + this
	);
}

SemanticValue operator-(const SemanticValue& lhs, const SemanticValue& rhs)
{
	return SemanticValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.bound() - arg2.bound(), arg1.size); },
		nullptr, //const - this
		[](SemanticThisPtr    arg1, SemanticKnownConst arg2) { return SemanticThisPtr{ arg1.offset - arg2.bound() }; },
		[](SemanticThisPtr    arg1, SemanticThisPtr    arg2) { return SemanticKnownConst(arg1.offset - arg2.offset, sizeof(void*)); }
	);
}

SemanticValue operator*(const SemanticValue& lhs, const SemanticValue& rhs)
{
	return SemanticValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.bound() * arg2.bound(), arg1.size); },
		nullptr, //const * this
		nullptr, //this * const
		nullptr //this * this
	);
}

#pragma endregion
