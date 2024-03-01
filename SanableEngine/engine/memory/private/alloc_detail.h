#pragma once

#if _MSC_VER
#include <malloc.h>
#define ALIGNED_ALLOC(size, align) _aligned_malloc(size, align)
#define ALIGNED_FREE(obj) _aligned_free(obj)
#else
#define ALIGNED_ALLOC(size, align) ::std::aligned_alloc(size, align)
#define ALIGNED_FREE(obj) ::std::free(obj)
#endif
