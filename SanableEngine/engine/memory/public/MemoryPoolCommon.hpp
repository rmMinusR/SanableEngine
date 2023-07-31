#pragma once

#include "dllapi.h"

#include <type_traits>
#include <cassert>

#pragma region From original MemoryPool by Dean Lawson

ENGINEMEM_API int isPowerOfTwo(size_t x);
ENGINEMEM_API size_t getClosestPowerOf2LargerThan(size_t num);

#pragma endregion

ENGINEMEM_API int roundUp(int n, int multiple);
