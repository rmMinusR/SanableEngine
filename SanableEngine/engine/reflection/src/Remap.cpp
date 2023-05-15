#include "Remap.hpp"

#include <cassert>

MemoryBlock<rel_addr_t> MemoryBlock<abs_addr_t>::toRel(void* object) const
{
	MemoryBlock<rel_addr_t> out;
	out.start = ((char*)start) - ((char*)object);
	out.length = this->length;
	return out;
}

bool MemoryBlock<abs_addr_t>::contains(abs_addr_t addr) const
{
	return start <= addr && addr < this->end();
}

bool MemoryBlock<abs_addr_t>::fullyContains(const MemoryBlock<abs_addr_t>& other) const
{
	return this->start <= other.start && other.end() <= this->end();
}

bool MemoryBlock<abs_addr_t>::intersects(const MemoryBlock<abs_addr_t>& other) const
{
	// Calculate common area
	void* lb = std::max(this->start, other.start);
	void* ub = std::min(this->end(), other.end());

	// Is common area valid?
	return lb < ub;
}

bool MemoryBlock<abs_addr_t>::operator==(const MemoryBlock<abs_addr_t>& other) const
{
	return other.start == this->start && other.length == this->length;
}

bool MemoryBlock<abs_addr_t>::operator!=(const MemoryBlock<abs_addr_t>& other) const
{
	return other.start != this->start || other.length != this->length;
}

bool MemoryBlock<rel_addr_t>::contains(rel_addr_t addr) const
{
	return start <= addr && addr < addr+length;
}

MemoryBlock<abs_addr_t> MemoryBlock<rel_addr_t>::toAbs(void* object) const
{
	MemoryBlock<abs_addr_t> out;
	out.start = ((char*)object) + start;
	out.length = this->length;
	return out;
}

bool MemoryBlock<rel_addr_t>::operator==(const MemoryBlock<rel_addr_t>& other) const
{
	return other.start == this->start && other.length == this->length;
}

bool MemoryBlock<rel_addr_t>::operator!=(const MemoryBlock<rel_addr_t>& other) const
{
	return other.start != this->start || other.length != this->length;
}

void RemappedBlock<abs_addr_t>::doMove()
{
	// Skip if move already performed
	if (moved) return;

	memcpy_s(to.start, to.length, from.start, from.length);
}

void* RemappedBlock<abs_addr_t>::updatePointer(void* ptr) const
{
	assert(from.contains(ptr));
	std::ptrdiff_t offset = ((char*)ptr) - ((char*)from.start);
	ptr = ((char*)to.start) + offset;
	assert(to.contains(ptr));
	return ptr;
}

RemappedBlock<rel_addr_t> RemappedBlock<abs_addr_t>::toRel(void* object) const
{
	RemappedBlock<rel_addr_t> out;
	out.from = this->from.toRel(object);
	out.to   = this->to  .toRel(object);
	return out;
}

bool RemappedBlock<abs_addr_t>::operator==(const RemappedBlock<abs_addr_t>& other) const
{
	return other.from == this->from && other.to == this->to;
}

bool RemappedBlock<abs_addr_t>::operator!=(const RemappedBlock<abs_addr_t>& other) const
{
	return other.from != this->from || other.to != this->to;
}

RemappedBlock<abs_addr_t> RemappedBlock<rel_addr_t>::toAbs(void* object) const
{
	RemappedBlock<abs_addr_t> out;
	out.from = this->from.toAbs(object);
	out.to   = this->to  .toAbs(object);
	return out;
}

bool RemappedBlock<rel_addr_t>::operator==(const RemappedBlock<rel_addr_t>& other) const
{
	return other.from == this->from && other.to == this->to;
}

bool RemappedBlock<rel_addr_t>::operator!=(const RemappedBlock<rel_addr_t>& other) const
{
	return other.from != this->from || other.to != this->to;
}

void LayoutRemapBuilder::enqueue(const RemappedBlock<abs_addr_t>& command)
{
	assert(std::find_if(buffer.cbegin(), buffer.cend(), [&](const RemappedBlock<abs_addr_t>& i) { return i.from == i.to; }) == buffer.cend());
	buffer.push_back(command); //TODO dependency-tree sort
}

void LayoutRemapBuilder::enqueue(const RemappedBlock<rel_addr_t>& command, void* object)
{
	enqueue(command.toAbs(object));
}

void LayoutRemap::moveBlocks()
{
	//TODO ensure sorted

	for (RemappedBlock<abs_addr_t>& cmd : buffer)
	{
		cmd.doMove();
	}
}

void* LayoutRemap::updatePointer(void* ptr) const
{
	for (const RemappedBlock<abs_addr_t>& cmd : buffer)
	{
		if (cmd.from.contains(ptr)) ptr = cmd.updatePointer(ptr); //TODO protect against overlapping from/to ranges
	}
	
	return ptr;
}
