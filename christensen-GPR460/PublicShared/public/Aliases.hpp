#pragma once

#include <string>

namespace gpr460
{
	typedef std::wstring string;
}

typedef void* vtable_func_ptr; //We don't know the type of a vtable function ptr
typedef vtable_func_ptr* vtable_ptr; //We don't know the type of a vtable ptr either

//VC++ puts the vtable ptr at the very start of the object's memory
inline vtable_ptr get_vtable_ptr(void const* obj) { return *reinterpret_cast<vtable_ptr const*>(obj); }
inline void set_vtable_ptr(void* obj, vtable_ptr vtable) { *reinterpret_cast<vtable_ptr*>(obj) = vtable; }
