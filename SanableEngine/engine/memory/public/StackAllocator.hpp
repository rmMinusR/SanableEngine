#pragma once

#include <cassert>

class StackAllocator
{
private:
    size_t maxSize;
    char* memory;
    size_t used;

public:
    StackAllocator();
    StackAllocator(size_t maxSize);
    ~StackAllocator();

    void resize(size_t newSize);

    void* allocRaw(size_t size);

    template <typename T>
    inline T* alloc(size_t arrayCount = 1) { return (T*)allocRaw(sizeof(T)*arrayCount); }

    class Checkpoint
    {
    private:
        size_t used = 0;
        friend class StackAllocator;
    };

    Checkpoint markCheckpoint();
    void restoreCheckpoint(const Checkpoint& c);
};
