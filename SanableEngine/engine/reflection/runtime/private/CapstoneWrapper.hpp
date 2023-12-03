#pragma once

#include "capstone/capstone.h"


void capstone_check_error(cs_err code);


csh capstone_get_instance(); //Lazy initializer
void capstone_cleanup_instance();


template<typename T1, typename T2>
bool carray_contains(T1* arr, size_t arrSize, T2 match)
{
	for (size_t i = 0; i < arrSize; ++i) if (arr[i] == match) return true;
	return false;
}

template<typename T1, typename TMatchFunc>
bool carray_contains_if(T1* arr, size_t arrSize, TMatchFunc matchFunc)
{
	for (size_t i = 0; i < arrSize; ++i) if (matchFunc(arr[i])) return true;
	return false;
}


typedef void(*void_func_ptr)(); //Cannot directly return a function ptr, must use a typedef otherwise lexer gets confused
void_func_ptr platform_getRelFunc(const cs_insn& insn);
void* platform_getRelAddr(const cs_insn& insn);


bool platform_isIf(const cs_insn& insn);


#if INTPTR_MAX == INT64_MAX
typedef uint64_t uint_addr_t;
#elif INTPTR_MAX == INT32_MAX
typedef uint32_t uint_addr_t;
#elif INTPTR_MAX == INT16_MAX
typedef uint16_t uint_addr_t;
#else
#error Unknown address space
#endif
