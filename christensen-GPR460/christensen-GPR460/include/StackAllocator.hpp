#pragma once

#include <cassert>

class StackAllocator
{
private:
    size_t maxSize;
    void* memory;
    void* head;

public:
    StackAllocator(size_t maxSize);
    ~StackAllocator();

    template <typename T>
    T* alloc() {
        T* ptr = head;
        head += sizeof(T);
        assert(head < memory+maxSize);
        return ptr;
    }

    template <typename T>
    T* alloc(size_t arrayCount) {
        T* ptr = head;
        head += sizeof(T)*arrayCount;
        assert(head < memory+maxSize);
        return ptr;
    }


    class Checkpoint
    {
    private:
        void* head;
        friend class StackAllocator;
    };

    Checkpoint markCheckpoint();
    void restoreCheckpoint(Checkpoint* c);
};
