#pragma once

#if WIN32
#include <malloc.h>
#define STACK_ALLOC _malloca
#define STACK_FREE _freea
#else
#include <cstdlib>
#define STACK_ALLOC alloca
#define STACK_FREE(obj)
#endif
