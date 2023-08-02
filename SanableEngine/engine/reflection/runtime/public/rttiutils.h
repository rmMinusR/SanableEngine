//No pragma once--include guards per compilation setting

//vtables: C-compatible

#ifndef SAN_RTTIUTILS_H_C
#define SAN_RTTIUTILS_H_C

typedef void* vtable_func_ptr; //We don't know the type of a vtable function ptr
typedef vtable_func_ptr* vtable_ptr; //We don't know the type of a vtable ptr either

//Pretty much all compilers put the vtable ptr at the very start of the object's memory
inline vtable_ptr get_vtable_ptr(void const* obj) { return *reinterpret_cast<vtable_ptr const*>(obj); }
inline void set_vtable_ptr(void* obj, vtable_ptr vtable) { *reinterpret_cast<vtable_ptr*>(obj) = vtable; }

typedef void (*dtor_t)(void*); //CANNOT be a std::function or lambda because destroying the dtor_t instance would attempt to delete memory from a possibly-unloaded plugin

#endif


//Destructor type erasure: C++ only

#ifdef __cplusplus
#ifndef SAN_RTTIUTILS_H_CXX
#define SAN_RTTIUTILS_H_CXX

#include <type_traits>

template<typename T, bool has_destructor = std::is_destructible<T>::value>
struct dtor_utils
{
	dtor_utils() = delete;
};

template<typename T>
struct dtor_utils<T, true>
{
	inline static void call_dtor(void* obj)
	{
		//C++ forbids getting the address of a dtor, but we can still wrap it
		static_cast<const T*>(obj)->~T();
	}

	constexpr static dtor_t dtor = &call_dtor;
};

template<typename T>
struct dtor_utils<T, false>
{
	//Can't call a dtor that doesn't exist
	constexpr static dtor_t dtor = nullptr;
};

#endif
#endif
