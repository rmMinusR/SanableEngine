#include "Remap.hpp"

#include <cassert>

PointerRange<rel_addr_t> PointerRange<abs_addr_t>::toRel(void* object) const
{
	PointerRange<rel_addr_t> out;
	out.start = ((char*)start) - ((char*)object);
	out.length = this->length;
	return out;
}

bool PointerRange<abs_addr_t>::contains(abs_addr_t addr) const
{
	return start <= addr && addr < this->end();
}

bool PointerRange<abs_addr_t>::fullyContains(const PointerRange<abs_addr_t>& other) const
{
	return this->start <= other.start && other.end() <= this->end();
}

bool PointerRange<abs_addr_t>::intersects(const PointerRange<abs_addr_t>& other) const
{
	return this->contains(other.start) || this->contains(other.end())
		|| other.contains(this->start) || other.contains(this->end());
}

bool PointerRange<abs_addr_t>::operator==(const PointerRange<abs_addr_t>& other) const
{
	return other.start == this->start && other.length == this->length;
}

bool PointerRange<abs_addr_t>::operator!=(const PointerRange<abs_addr_t>& other) const
{
	return other.start != this->start || other.length != this->length;
}

bool PointerRange<rel_addr_t>::contains(rel_addr_t addr) const
{
	return start <= addr && addr < addr+length;
}

PointerRange<abs_addr_t> PointerRange<rel_addr_t>::toAbs(void* object) const
{
	PointerRange<abs_addr_t> out;
	out.start = ((char*)object) + start;
	out.length = this->length;
	return out;
}

bool PointerRange<rel_addr_t>::operator==(const PointerRange<rel_addr_t>& other) const
{
	return other.start == this->start && other.length == this->length;
}

bool PointerRange<rel_addr_t>::operator!=(const PointerRange<rel_addr_t>& other) const
{
	return other.start != this->start || other.length != this->length;
}

void RemapCommand<abs_addr_t>::doMove()
{
	// Skip if move already performed
	if (moved) return;

	memcpy_s(to.start, to.length, from.start, from.length);
}

void* RemapCommand<abs_addr_t>::updatePointer(void* ptr) const
{
	assert(from.contains(ptr));
	std::ptrdiff_t offset = ((char*)ptr) - ((char*)from.start);
	ptr = ((char*)to.start) + offset;
	assert(to.contains(ptr));
	return ptr;
}

RemapCommand<rel_addr_t> RemapCommand<abs_addr_t>::toRel(void* object) const
{
	RemapCommand<rel_addr_t> out;
	out.from = this->from.toRel(object);
	out.to   = this->to  .toRel(object);
	return out;
}

bool RemapCommand<abs_addr_t>::operator==(const RemapCommand<abs_addr_t>& other) const
{
	return other.from == this->from && other.to == this->to;
}

bool RemapCommand<abs_addr_t>::operator!=(const RemapCommand<abs_addr_t>& other) const
{
	return other.from != this->from || other.to != this->to;
}

RemapCommand<abs_addr_t> RemapCommand<rel_addr_t>::toAbs(void* object) const
{
	RemapCommand<abs_addr_t> out;
	out.from = this->from.toAbs(object);
	out.to   = this->to  .toAbs(object);
	return out;
}

bool RemapCommand<rel_addr_t>::operator==(const RemapCommand<rel_addr_t>& other) const
{
	return other.from == this->from && other.to == this->to;
}

bool RemapCommand<rel_addr_t>::operator!=(const RemapCommand<rel_addr_t>& other) const
{
	return other.from != this->from || other.to != this->to;
}

void LayoutRemapCommandBuffer::enqueue(const RemapCommand<abs_addr_t>& command)
{
	assert(std::find_if(buffer.cbegin(), buffer.cend(), [&](const RemapCommand<abs_addr_t>& i) { return i.from == i.to; }) == buffer.cend());
	buffer.push_back(command); //TODO dependency-tree sort
}

void LayoutRemapCommandBuffer::enqueue(const RemapCommand<rel_addr_t>& command, void* object)
{
	enqueue(command.toAbs(object));
}

void LayoutRemapCommandBuffer::doMove()
{
	//TODO ensure sorted

	for (RemapCommand<abs_addr_t>& cmd : buffer)
	{
		cmd.doMove();
	}
}

void* LayoutRemapCommandBuffer::updatePointer(void* ptr) const
{
	for (const RemapCommand<abs_addr_t>& cmd : buffer)
	{
		if (cmd.from.contains(ptr)) ptr = cmd.updatePointer(ptr); //TODO protect against overlapping from/to ranges
	}
	
	return ptr;
}
