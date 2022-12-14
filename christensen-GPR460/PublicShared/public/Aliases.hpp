#pragma once

#include <string>

namespace gpr460
{
	typedef std::wstring string;
}

typedef void* vtable_func_ptr; //We don't know the type of a vtable function ptr
typedef vtable_func_ptr* vtable_ptr; //We don't know the type of a vtable ptr either

inline vtable_ptr get_vtable_ptr(void* obj) { return *reinterpret_cast<vtable_ptr*>(obj); } //VC++ puts the vtable ptr at the very start of the object's memory
