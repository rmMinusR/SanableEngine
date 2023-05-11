#include "StackAllocator.hpp"

#include <cstdlib>

StackAllocator::StackAllocator() :
    maxSize(0),
    memory(nullptr),
    used(0)
{
}

StackAllocator::StackAllocator(size_t bytesToAllocate) : StackAllocator()
{
    resize(bytesToAllocate);
}

StackAllocator::~StackAllocator()
{
    if (memory)
    {
        free(memory);
        memory = nullptr;
    }
}

void StackAllocator::resize(size_t newSize)
{
    assert(!used); // Only allow resizing if we're empty, otherwise previous allocations will be dangling pointers
    maxSize = newSize;
    memory = (char*)realloc(memory, newSize);
}

void* StackAllocator::allocRaw(size_t size)
{
    assert(used + size <= maxSize);
    void* ptr = memory + used;
    used += size;
    return ptr;
}

StackAllocator::Checkpoint StackAllocator::markCheckpoint()
{
    Checkpoint c;
    c.used = this->used;
    return c;
}

void StackAllocator::restoreCheckpoint(const Checkpoint& c)
{
    assert(used >= c.used);
    used = c.used;
}
