#pragma once

#include <cstdint>
#include <cstring>
#include "dllapi.h"

template<typename Ret, typename... Args>
union _Fn
{
	typedef Ret(*ptr_t)(Args...);

	inline _Fn(uint8_t* data) { this->data = data; }
	inline _Fn(ptr_t fn) { this->fn = fn; }
	
	uint8_t* data;
	ptr_t fn;

	inline operator ptr_t() { return fn; }
};


typedef ptrdiff_t thunk_marker_t;

//Convenience wrappers
ENGINE_RTTI_API thunk_marker_t scanForNearCall(_Fn<void> toScan, _Fn<void> searchFor, thunk_marker_t prevCall = 0);
ENGINE_RTTI_API _Fn<void>::ptr_t getNearCallFnPtr(_Fn<void> thunk, thunk_marker_t marker);
