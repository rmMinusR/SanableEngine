#pragma once

#include <cassert>
#include <cstddef>

#include "dllapi.h"

class StackAllocator
{
private:
    size_t maxSize;
    size_t used;
    char* memory;

public:
    ENGINEMEM_API StackAllocator();
    ENGINEMEM_API StackAllocator(size_t maxSize);
    ENGINEMEM_API ~StackAllocator();

    ENGINEMEM_API void resize(size_t newSize);

    ENGINEMEM_API void* allocRaw(size_t size);

    template <typename T>
    inline T* alloc(size_t arrayCount = 1) { return (T*)allocRaw(sizeof(T)*arrayCount); }

    class Checkpoint
    {
    private:
        size_t used = 0;
        friend class StackAllocator;
    };

    ENGINEMEM_API Checkpoint markCheckpoint();
    ENGINEMEM_API void restoreCheckpoint(const Checkpoint& c);
};
