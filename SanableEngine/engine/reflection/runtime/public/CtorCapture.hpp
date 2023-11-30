#pragma once

#include "dllapi.h"

ENGINE_RTTI_API void scanForVtables(void(*ctorThunk)());

template<typename T, typename... Args>
struct ctor_utils
{
	static void newOnStack(Args... args)
	{
		T v(args...);
	}

	static void newOnHeap(Args... args)
	{
		new T(args...);
	}
	
	static void newInPlace(T* ptr, Args... args)
	{
		new(ptr) T(args...);
	}

	ctor_utils() = delete;
};
