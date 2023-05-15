#pragma once

#include <vector>

#include "dllapi.h"

typedef void* abs_addr_t;
typedef std::ptrdiff_t rel_addr_t;

#pragma region PointerRange

template<typename addr_t>
struct PointerRange;

template<>
struct PointerRange<abs_addr_t>
{
	abs_addr_t start = nullptr;
	size_t length = 0;
	inline abs_addr_t end() const { return ((char*)start)+length; } // NOTE: Exclusive, does NOT include this pointer

	ENGINEREFL_API PointerRange<rel_addr_t> toRel(void* object) const;
	ENGINEREFL_API bool contains(abs_addr_t addr) const;

	ENGINEREFL_API bool fullyContains(const PointerRange<abs_addr_t>& other) const;
	ENGINEREFL_API bool intersects(const PointerRange<abs_addr_t>& other) const;

	ENGINEREFL_API bool operator==(const PointerRange<abs_addr_t>& other) const;
	ENGINEREFL_API bool operator!=(const PointerRange<abs_addr_t>& other) const;
};

template<>
struct PointerRange<rel_addr_t>
{
	rel_addr_t start = 0;
	size_t length = 0;

	ENGINEREFL_API PointerRange<abs_addr_t> toAbs(void* object) const;
	ENGINEREFL_API bool contains(rel_addr_t addr) const;

	ENGINEREFL_API bool operator==(const PointerRange<rel_addr_t>& other) const;
	ENGINEREFL_API bool operator!=(const PointerRange<rel_addr_t>& other) const;
};

#pragma endregion

#pragma region RemapCommand

template<typename addr_t>
struct RemapCommand;

template<>
struct RemapCommand<abs_addr_t>
{
	PointerRange<abs_addr_t> from, to;
	bool moved = false;

	ENGINEREFL_API void doMove(); // NOTE: Skips if move already performed
	ENGINEREFL_API void* updatePointer(void* ptr) const;

	ENGINEREFL_API RemapCommand<rel_addr_t> toRel(void* object) const;

	ENGINEREFL_API bool operator==(const RemapCommand<abs_addr_t>& other) const;
	ENGINEREFL_API bool operator!=(const RemapCommand<abs_addr_t>& other) const;
};

template<>
struct RemapCommand<rel_addr_t>
{
	PointerRange<rel_addr_t> from, to;

	ENGINEREFL_API RemapCommand<abs_addr_t> toAbs(void* object) const;

	ENGINEREFL_API bool operator==(const RemapCommand<rel_addr_t>& other) const;
	ENGINEREFL_API bool operator!=(const RemapCommand<rel_addr_t>& other) const;
};

#pragma endregion

class LayoutRemapCommandBuffer
{
private:
	std::vector<RemapCommand<abs_addr_t>> buffer;

public:
	ENGINEREFL_API void enqueue(const RemapCommand<abs_addr_t>& command);
	ENGINEREFL_API void enqueue(const RemapCommand<rel_addr_t>& command, void* object);

	ENGINEREFL_API void doMove();
	ENGINEREFL_API void* updatePointer(void* ptr) const;
};



#include "FieldInfo.hpp"

struct LayoutRemap
{
	ENGINEREFL_API void execute(void* obj);
	ENGINEREFL_API void doSanityCheck(); //Complain if new members are introduced, or old members are deleted
private:
	size_t swapSize; //We need to make a temporary allocation with this algorithm
	std::vector<std::pair<const FieldInfo*, const FieldInfo*>> contents;
	friend class TypeInfo;
};