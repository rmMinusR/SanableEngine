#include "SemanticValue.hpp"

#include <cassert>
#include <functional>


int debugPrintSignedHex(int64_t val)
{
	if (val >= 0) return printf("+0x%llx", val);
	else          return printf("-0x%llx", 1 + (~uint64_t(0) - uint64_t(val)));
}


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

std::optional<bool> SemanticFlags::check(int id, bool expectedValue) const
{
	uint64_t mask = 1<<id;
	return (bitsKnown&mask) ? std::make_optional<bool>( (bits&mask) == expectedValue) : std::nullopt;
}

void SemanticFlags::set(int id, std::optional<bool> val)
{
	uint64_t mask = 1<<id;
	if (val.has_value())
	{
		if (val.value()) bits |= mask;
		else bits &= ~mask;
		bitsKnown |= mask;
	}
	else bitsKnown &= ~mask;
}

bool SemanticFlags::allKnown(const std::initializer_list<int>& ids) const
{
	uint64_t mask = 0;
	for (int id : ids) mask |= 1<<id;
	return (bitsKnown&mask) == mask;
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

//Getters
#define _X(id) \
	Semantic##id* SemanticValue::tryGet##id() \
	{ \
		if (valueType == Type::id) return &as##id; \
		else return nullptr; \
	} \
	const Semantic##id* SemanticValue::tryGet##id() const \
	{ \
		if (valueType == Type::id) return &as##id; \
		else return nullptr; \
	}

_XM_FOREACH_SEMANTICVALUE_TYPE_EXCEPT_UNKNOWN()
#undef _X

int SemanticValue::debugPrintValue() const
{
	int nWritten = 0;

	     if (const SemanticKnownConst* _val = tryGetKnownConst()) nWritten = debugPrintSignedHex(_val->value);
	else if (const SemanticThisPtr   * _val = tryGetThisPtr   ()) nWritten = printf("this") + debugPrintSignedHex(_val->offset);
	else if (isUnknown()) nWritten = printf("(unknown)");
	else assert(false);

	int nToPad = 15-nWritten;
	if (nToPad > 0) printf("%*c", nToPad, ' ');

	return nWritten + std::max(0, nToPad);
}

SemanticValue::SemanticValue() : SemanticValue(SemanticUnknown(0)) { }

//Converting constructors and operator=
#define _X(id) \
	SemanticValue::SemanticValue(const Semantic##id& val) { *this = val; } \
	SemanticValue& SemanticValue::operator=(const Semantic##id& val) \
	{ \
		valueType = Type::id; \
		as##id = val; \
		return *this; \
	}

_XM_FOREACH_SEMANTICVALUE_TYPE()
#undef _X


#pragma region Math

SemanticValue SemanticValue_doMathOp(SemanticValue arg1, SemanticValue arg2,
	const std::function<SemanticValue(SemanticKnownConst, SemanticKnownConst)>& funcConstConst, //Any of these can be null and we'll just return default unknown
	const std::function<SemanticValue(SemanticKnownConst, SemanticThisPtr   )>& funcConstThis,
	const std::function<SemanticValue(SemanticThisPtr   , SemanticKnownConst)>& funcThisConst,
	const std::function<SemanticValue(SemanticThisPtr   , SemanticThisPtr   )>& funcThisThis,
	const std::function<SemanticValue(SemanticFlags     , SemanticFlags     )>& funcFlagsFlags)
{
	auto* const1 = arg1.tryGetKnownConst();
	auto* const2 = arg2.tryGetKnownConst();
	auto* pThis1 = arg1.tryGetThisPtr();
	auto* pThis2 = arg2.tryGetThisPtr();
	auto* flags1 = arg2.tryGetFlags();
	auto* flags2 = arg2.tryGetFlags();

	//If entire flags field is known, it is effectively a known const
	if (flags1 && flags1->bitsKnown == ~0ull) const1 = reinterpret_cast<SemanticKnownConst*>(flags1); //const's value corresponds to flags' bits field
	if (flags2 && flags2->bitsKnown == ~0ull) const2 = reinterpret_cast<SemanticKnownConst*>(flags2);

	SemanticUnknown unknown(std::max(arg1.getSize(), arg2.getSize()));
	if (arg1.isUnknown() || arg2.isUnknown()) return unknown;
	else if (const1 && const2)
	{
		assert(const1->size == const2->size);
		return funcConstConst ? funcConstConst(*const1, *const2) : unknown;
	}
	else if (const1 && pThis2) return funcConstThis  ? funcConstThis (*const1, *pThis2) : unknown;
	else if (pThis1 && const2) return funcThisConst  ? funcThisConst (*pThis1, *const2) : unknown;
	else if (pThis1 && pThis2) return funcThisThis   ? funcThisThis  (*pThis1, *pThis2) : unknown;
	else if (flags1 && flags2) return funcFlagsFlags ? funcFlagsFlags(*flags1, *flags2) : unknown;
	else { assert(false); return SemanticValue(); }
}

SemanticValue operator+(const SemanticValue& lhs, const SemanticValue& rhs)
{
	return SemanticValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.bound() + arg2.bound(), arg1.size); },
		[](SemanticKnownConst arg1, SemanticThisPtr    arg2) { return SemanticThisPtr{ arg1.bound() + arg2.offset}; },
		[](SemanticThisPtr    arg1, SemanticKnownConst arg2) { return SemanticThisPtr{ arg1.offset + arg2.bound() }; },
		nullptr, //this + this
		nullptr //flags + flags
	);
}

SemanticValue operator-(const SemanticValue& lhs, const SemanticValue& rhs)
{
	return SemanticValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.bound() - arg2.bound(), arg1.size); },
		nullptr, //const - this
		[](SemanticThisPtr    arg1, SemanticKnownConst arg2) { return SemanticThisPtr{ arg1.offset - arg2.bound() }; },
		[](SemanticThisPtr    arg1, SemanticThisPtr    arg2) { return SemanticKnownConst(arg1.offset - arg2.offset, sizeof(void*)); },
		nullptr //flags + flags
	);
}

SemanticValue operator*(const SemanticValue& lhs, const SemanticValue& rhs)
{
	return SemanticValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.bound() * arg2.bound(), arg1.size); },
		nullptr, //const * this
		nullptr, //this * const
		nullptr, //this * this
		nullptr //flags * flags
	);
}

#pragma endregion
