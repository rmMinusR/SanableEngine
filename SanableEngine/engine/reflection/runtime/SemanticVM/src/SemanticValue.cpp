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

SemanticKnownConst::SemanticKnownConst(uint64_t v, size_t s, bool isPositionIndependentAddr) :
	size(s),
	value(v),
	isPositionIndependentAddr(isPositionIndependentAddr)
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

uint64_t SemanticKnownConst::mask() const
{
	return ~(~0ull << size);
}

SemanticKnownConst SemanticKnownConst::signExtend(size_t targetSizeBytes) const
{
	assert(!isPositionIndependentAddr);

	int64_t val = 0;
	     if (size == 1) val = int8_t ((int64_t)value);
	else if (size == 2) val = int16_t((int64_t)value);
	else if (size == 4) val = int32_t((int64_t)value);
	else if (size == 8) val = int64_t((int64_t)value);
	else assert(false);
	return SemanticKnownConst(val, targetSizeBytes, false);
}

SemanticKnownConst SemanticKnownConst::zeroExtend(size_t targetSizeBytes) const
{
	uint64_t val = 0;
	     if (size == 1) val = uint8_t ((uint64_t)value);
	else if (size == 2) val = uint16_t((uint64_t)value);
	else if (size == 4) val = uint32_t((uint64_t)value);
	else if (size == 8) val = uint64_t((uint64_t)value);
	else assert(false);
	return SemanticKnownConst(val, targetSizeBytes, false);
}

bool SemanticKnownConst::isSigned() const
{
	     if (size == 1) return int8_t ((int64_t)value) < 0;
	else if (size == 2) return int16_t((int64_t)value) < 0;
	else if (size == 4) return int32_t((int64_t)value) < 0;
	else if (size == 8) return int64_t((int64_t)value) < 0;
	else { assert(false); return false; }
}

void SemanticKnownConst::setSign(bool sign)
{
	value = abs(asSigned());
	if (sign) value = -value;
}

int64_t SemanticKnownConst::asSigned() const
{
	return (int64_t)signExtend(sizeof(int64_t)).value;
}

SemanticMagic::SemanticMagic(size_t size, size_t offset, id_t id) :
	size(size),
	offset(offset),
	id(id)
{
}

SemanticFlags::SemanticFlags()
{
}

SemanticFlags::SemanticFlags(uint64_t bits, uint64_t known) :
	bits(bits),
	bitsKnown(known)
{
}

std::optional<bool> SemanticFlags::check(int id, bool expectedValue) const
{
	uint64_t mask = 1<<id;
	return (bitsKnown&mask) ? std::make_optional<bool>( bool(bits&mask) == expectedValue) : std::nullopt;
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

std::optional<bool> SemanticValue::isZero() const
{
	if (isUnknown()) return std::nullopt;

	switch (valueType)
	{
	case Type::KnownConst: return asKnownConst.bound() == 0;
	case Type::Magic: return false;
	case Type::Flags: return (asFlags.bitsKnown == ~0ull) ? std::make_optional<bool>(asFlags.bits) == 0 : std::make_optional<bool>();

	default: assert(false && "Unhandled value type"); return std::nullopt;
	}
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

int SemanticValue::debugPrintValue(bool pad) const
{
	int nWritten = 0;

	     if (const SemanticKnownConst* _val = tryGetKnownConst()) nWritten = debugPrintSignedHex(_val->value);
	else if (const SemanticMagic     * _val = tryGetMagic     ()) nWritten = printf("magic[%i]", _val->id) + debugPrintSignedHex(_val->offset);
	else if (isUnknown()) nWritten = printf("(unknown)");
	else assert(false);

	int nToPad = 15-nWritten;
	if (pad && nToPad > 0) nWritten += printf("%*c", nToPad, ' ');

	return nWritten;
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


bool SemanticValue::operator==(const SemanticValue& rhs) const
{
	return this->valueType == rhs.valueType
		&& memcmp(&this->asUnknown, &rhs.asUnknown, variantSize) == 0; //FIXME fast impl, should really call each value type's operator==
}

bool SemanticValue::operator!=(const SemanticValue& rhs) const
{
	return !(*this == rhs);
}


#pragma region Math

SemanticValue SemanticValue_doMathOp(SemanticValue arg1, SemanticValue arg2,
	const std::function<SemanticValue(SemanticKnownConst, SemanticKnownConst)>& funcConstConst, //Any of these can be null and we'll just return default unknown
	const std::function<SemanticValue(SemanticKnownConst, SemanticMagic   )>& funcConstThis,
	const std::function<SemanticValue(SemanticMagic   , SemanticKnownConst)>& funcThisConst,
	const std::function<SemanticValue(SemanticMagic   , SemanticMagic   )>& funcThisThis,
	const std::function<SemanticValue(SemanticFlags     , SemanticFlags     )>& funcFlagsFlags)
{
	auto* const1 = arg1.tryGetKnownConst();
	auto* const2 = arg2.tryGetKnownConst();
	auto* pMagic1 = arg1.tryGetMagic();
	auto* pMagic2 = arg2.tryGetMagic();
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
	else if (const1  && pMagic2) return funcConstThis  ? funcConstThis (*const1 , *pMagic2) : unknown;
	else if (pMagic1 && const2 ) return funcThisConst  ? funcThisConst (*pMagic1, *const2 ) : unknown;
	else if (pMagic1 && pMagic2) return funcThisThis   ? funcThisThis  (*pMagic1, *pMagic2) : unknown;
	else if (flags1  && flags2 ) return funcFlagsFlags ? funcFlagsFlags(*flags1 , *flags2 ) : unknown;
	else { assert(false); return SemanticValue(); }
}

SemanticValue operator+(const SemanticValue& lhs, const SemanticValue& rhs)
{
	return SemanticValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.bound() + arg2.bound(), arg1.size, (arg1.isPositionIndependentAddr + arg2.isPositionIndependentAddr) == 1); },
		[](SemanticKnownConst arg1, SemanticMagic      arg2) { return SemanticMagic(arg2.size, arg2.offset+arg1.bound(), arg2.id); },
		[](SemanticMagic      arg1, SemanticKnownConst arg2) { return SemanticMagic(arg1.size, arg1.offset+arg2.bound(), arg1.id); },
		nullptr, //this + this
		nullptr //flags + flags
	);
}

SemanticValue operator-(const SemanticValue& lhs, const SemanticValue& rhs)
{
	if (lhs == rhs) return SemanticKnownConst(0, lhs.getSize(), false);
	return SemanticValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.bound() - arg2.bound(), arg1.size, (arg1.isPositionIndependentAddr - arg2.isPositionIndependentAddr) == 1); },
		nullptr, //const - this
		[](SemanticMagic arg1, SemanticKnownConst arg2) { return SemanticMagic(arg1.size, arg1.offset-arg2.bound(), arg1.id); },
		[](SemanticMagic arg1, SemanticMagic      arg2) {
			SemanticValue v;
			if (arg1.id == arg2.id) v = SemanticKnownConst(arg1.offset-arg2.offset, arg1.size, false);
			else v = SemanticUnknown(arg1.size);
			return v;
		},
		nullptr //flags + flags
	);
}

SemanticValue operator&(const SemanticValue& lhs, const SemanticValue& rhs)
{
	if (lhs == rhs) return lhs;
	return SemanticValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.bound() & arg2.bound(), arg1.size, false); },
		nullptr, //const & this
		nullptr, //this & const
		nullptr, //this & this: identity case will catch most, otherwise it actually holds no semantic meaning
		[](SemanticFlags arg1, SemanticFlags arg2) { return SemanticFlags(arg1.bits & arg2.bits, arg1.bitsKnown & arg2.bitsKnown); }
	);
}

SemanticValue operator|(const SemanticValue& lhs, const SemanticValue& rhs)
{
	if (lhs == rhs) return lhs;
	return SemanticValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.bound() | arg2.bound(), arg1.size, false); },
		nullptr, //const & this
		nullptr, //this & const
		nullptr, //this & this: identity case will catch most, otherwise it actually holds no semantic meaning
		[](SemanticFlags arg1, SemanticFlags arg2) { return SemanticFlags(arg1.bits | arg2.bits, arg1.bitsKnown & arg2.bitsKnown); }
	);
}

SemanticValue operator^(const SemanticValue& lhs, const SemanticValue& rhs)
{
	if (lhs == rhs && !lhs.isUnknown()) return SemanticKnownConst(0, lhs.getSize(), false);
	return SemanticValue_doMathOp(lhs, rhs,
		[](SemanticKnownConst arg1, SemanticKnownConst arg2) { return SemanticKnownConst(arg1.bound() ^ arg2.bound(), arg1.size, false); },
		nullptr, //const & this
		nullptr, //this & const
		nullptr, //this & this: identity case will catch most, otherwise it actually holds no semantic meaning
		[](SemanticFlags arg1, SemanticFlags arg2) { return SemanticFlags(arg1.bits ^ arg2.bits, arg1.bitsKnown & arg2.bitsKnown); }
	);
}

#pragma endregion
