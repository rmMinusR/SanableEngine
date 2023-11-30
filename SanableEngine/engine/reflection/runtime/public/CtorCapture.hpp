#pragma once

#include "dllapi.h"

ENGINE_RTTI_API void scanForVtables(void(*thunk)());

template<typename T, typename... Args>
struct ctor_thunk
{
	static T* newOnHeap(Args... args)
	{
		return new T(args...);
	}

	static void newInPlace(T* ptr, Args... args)
	{
		return new(ptr) T(args...);
	}

	ctor_thunk() = delete;
};
