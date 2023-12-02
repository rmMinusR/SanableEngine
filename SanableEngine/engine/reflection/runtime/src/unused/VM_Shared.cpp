#include "VM.hpp"

SemanticKnownConst::~SemanticKnownConst()
{
	if (ownsValue) delete value;
}

SemanticKnownConst::SemanticKnownConst(const SemanticKnownConst& cpy) { *this = cpy; }
SemanticKnownConst::SemanticKnownConst(SemanticKnownConst&& mov) { *this = std::move(mov); }

SemanticKnownConst& SemanticKnownConst::operator=(const SemanticKnownConst& cpy)
{
	size = cpy.size;
	value = (uint8_t*)malloc(cpy.size);
	memcpy(value, cpy.value, size);
	ownsValue = cpy.ownsValue;

	return *this;
}

SemanticKnownConst& SemanticKnownConst::operator=(SemanticKnownConst&& mov)
{
	memcpy(this, &mov, sizeof(*this)); //Transfer data and ownership
	memset(&mov, 0, sizeof(*this)); //Clear original

	return *this;
}

SemanticKnownConst&& SemanticKnownConst::slice(size_t start, size_t end) const
{
	SemanticKnownConst out;
	out.size = end-start;
	out.value = value+start;
	out.ownsValue = false;
	return std::move(out);
}
