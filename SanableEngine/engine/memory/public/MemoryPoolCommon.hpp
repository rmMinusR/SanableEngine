#pragma once

#include "dllapi.h"

#include <type_traits>
#include <cassert>

#pragma region Destructor template magick

template<typename TObj, bool has_destructor = std::is_destructible<TObj>::value>
struct optional_destructor {
    optional_destructor() = delete;
};

template<typename TObj>
struct optional_destructor<TObj, true> {
    static void call(TObj* obj) {
        //Dtor present, call it
        obj->~TObj();
    }
};

template<typename TObj>
struct optional_destructor<TObj, false> {
    static void call(TObj* obj) {
        //No dtor present, nothing to do
    }
};

#pragma endregion

#pragma region From original MemoryPool by Dean Lawson

int isPowerOfTwo(size_t x);
size_t getClosestPowerOf2LargerThan(size_t num);

#pragma endregion