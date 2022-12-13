#pragma once

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#include "System.hpp"

#ifdef __EMSCRIPTEN__

#include "System_Emscripten.hpp"
namespace gpr460 {
	typedef System_Emscripten System_Impl;
}

#else

#include "System_Win32.hpp"
namespace gpr460 {
	typedef System_Win32 System_Impl;
}

#endif