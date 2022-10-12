#pragma once

#include "System_Outline.hpp"

#ifdef __EMSCRIPTEN__

#include "System_Emscripten.hpp"
namespace gpr460 {
	typedef System_Emscripten System;
}

#else

#include "System_Win32.hpp"
namespace gpr460 {
	typedef System_Win32 System;
}

#endif