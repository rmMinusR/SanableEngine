#pragma once

#include <vector>

#include "dllapi.h"

typedef void* abs_addr_t;
typedef std::ptrdiff_t rel_addr_t;

#pragma region MemoryBlock

template<typename addr_t>
struct MemoryBlock;

template<>
struct MemoryBlock<abs_addr_t>
{
	abs_addr_t start = nullptr;
	size_t length = 0;
	inline abs_addr_t end() const { return ((char*)start)+length; } // NOTE: Exclusive, does NOT include this pointer

	ENGINEREFL_API MemoryBlock<rel_addr_t> toRel(void* object) const;
	ENGINEREFL_API bool contains(abs_addr_t addr) const;

	ENGINEREFL_API bool fullyContains(const MemoryBlock<abs_addr_t>& other) const;
	ENGINEREFL_API bool intersects(const MemoryBlock<abs_addr_t>& other) const;

	ENGINEREFL_API bool operator==(const MemoryBlock<abs_addr_t>& other) const;
	ENGINEREFL_API bool operator!=(const MemoryBlock<abs_addr_t>& other) const;
};

template<>
struct MemoryBlock<rel_addr_t>
{
	rel_addr_t start = 0;
	size_t length = 0;

	ENGINEREFL_API MemoryBlock<abs_addr_t> toAbs(void* object) const;
	ENGINEREFL_API bool contains(rel_addr_t addr) const;

	ENGINEREFL_API bool operator==(const MemoryBlock<rel_addr_t>& other) const;
	ENGINEREFL_API bool operator!=(const MemoryBlock<rel_addr_t>& other) const;
};

#pragma endregion

#pragma region RemappedBlock

template<typename addr_t>
struct RemappedBlock;

template<>
struct RemappedBlock<abs_addr_t>
{
	MemoryBlock<abs_addr_t> from, to;
	bool moved = false;

	ENGINEREFL_API void doMove(); // NOTE: Skips if move already performed
	ENGINEREFL_API void* updatePointer(void* ptr) const;

	ENGINEREFL_API RemappedBlock<rel_addr_t> toRel(void* object) const;

	ENGINEREFL_API bool operator==(const RemappedBlock<abs_addr_t>& other) const;
	ENGINEREFL_API bool operator!=(const RemappedBlock<abs_addr_t>& other) const;
};

template<>
struct RemappedBlock<rel_addr_t>
{
	MemoryBlock<rel_addr_t> from, to;

	ENGINEREFL_API RemappedBlock<abs_addr_t> toAbs(void* object) const;

	ENGINEREFL_API bool operator==(const RemappedBlock<rel_addr_t>& other) const;
	ENGINEREFL_API bool operator!=(const RemappedBlock<rel_addr_t>& other) const;
};

#pragma endregion

class LayoutRemapBuilder;

class LayoutRemap
{
private:
	std::vector<RemappedBlock<abs_addr_t>> buffer; //Guaranteed flat and in order
	bool haveBlocksMoved = false;

	friend class LayoutRemapBuilder;

public:
	ENGINEREFL_API void moveBlocks();
	ENGINEREFL_API void* updatePointer(void* ptr) const;
};

class LayoutRemapBuilder
{
private:
	std::vector<RemappedBlock<abs_addr_t>> buffer;

public:
	ENGINEREFL_API void enqueue(const RemappedBlock<abs_addr_t>& command);
	ENGINEREFL_API void enqueue(const RemappedBlock<rel_addr_t>& command, void* object);

	ENGINEREFL_API void doSanityCheck(); //Complain if new stuff was introduced, or old members are deleted
	ENGINEREFL_API LayoutRemap build() const;
};
