#pragma once

#if WIN32
#include <malloc.h>
#define STACK_ALLOC _malloca
#else
#include <alloca.h>
#define STACK_ALLOC alloca
#endif
