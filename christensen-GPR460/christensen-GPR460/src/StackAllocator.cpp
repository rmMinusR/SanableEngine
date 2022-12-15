#include "StackAllocator.hpp"

#include <cstdlib>

StackAllocator::StackAllocator(size_t bytesToAllocate)
{
    maxSize = bytesToAllocate;
    head = memory = (uint8_t*)malloc(bytesToAllocate);
}

StackAllocator::~StackAllocator()
{
    free(memory);
}

StackAllocator::Checkpoint StackAllocator::markCheckpoint()
{
    Checkpoint c;
    c.head = this->head;
    return c;
}

void StackAllocator::restoreCheckpoint(const Checkpoint& c)
{
    assert(head >= c.head);
    head = c.head;
}
