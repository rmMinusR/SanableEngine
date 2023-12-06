#pragma once

#include <map>
#include <cstdint>

#include "SemanticValue.hpp"

/// <summary>
/// Represents an emulated memory space. Multiple of these can be combined (ie. ThisPtr is treated as its own memory space)
/// </summary>
class VMMemory
{
	std::map<uint8_t*, SemanticValue> memory; //Will never be SemanticUnknown
public:
	void reset();

	SemanticValue get(void* location, size_t size) const;
	void set(void* location, SemanticValue value, size_t size);

	inline SemanticValue get(uint64_t location, size_t size) const { return get((void*)location, size); }
	inline void set(uint64_t location, SemanticValue value, size_t size) { set((void*)location, value, size); }
};
