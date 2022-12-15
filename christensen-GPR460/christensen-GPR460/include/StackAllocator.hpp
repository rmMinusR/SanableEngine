#pragma once

#include <cassert>
#include <cstdint>

class StackAllocator
{
private:
    size_t maxSize;
    uint8_t* memory;
    uint8_t* head;

public:
    StackAllocator(size_t maxSize);
    ~StackAllocator();

    template <typename T>
    T* alloc() {
        T* ptr = (T*)head;
        head += sizeof(T);
        assert(head < memory+maxSize);
        return ptr;
    }

    template <typename T>
    T* alloc(size_t arrayCount) {
        T* ptr = (T*)head;
        head += sizeof(T)*arrayCount;
        assert(head < memory+maxSize);
        return ptr;
    }


    class Checkpoint
    {
    private:
        uint8_t* head;
        friend class StackAllocator;
    };

    Checkpoint markCheckpoint();
    void restoreCheckpoint(const Checkpoint& c);
};
