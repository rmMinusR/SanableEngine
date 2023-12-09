#pragma once

#include <map>
#include <cstdint>

#include "SemanticValue.hpp"

struct MachineState;

/// <summary>
/// Represents an emulated memory space. Multiple of these can be combined (ie. ThisPtr is treated as its own memory space)
/// </summary>
class VMMemory
{
	friend struct MachineState;
	std::map<uint8_t*, SemanticValue> memory;
public:
	void reset();

	[[nodiscard]] SemanticValue get(void* location, size_t size) const;
	void set(void* location, SemanticValue value, size_t size);

	inline SemanticValue get(uint64_t location, size_t size) const { return get((void*)location, size); }
	inline void set(uint64_t location, SemanticValue value, size_t size) { set((void*)location, value, size); }
};
