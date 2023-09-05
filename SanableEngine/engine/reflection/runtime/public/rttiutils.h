#pragma once

#include <type_traits>

typedef void const* vtable_ptr; //We don't know the type of a vtable ptr either

//Pretty much all compilers put the vtable ptr at the very start of the object's memory
inline vtable_ptr get_vtable_ptr(void const* obj) { return *reinterpret_cast<vtable_ptr const*>(obj); }

typedef void (*dtor_t)(void*); //CANNOT be a std::function or lambda because destroying the dtor_t instance would attempt to delete memory from a possibly-unloaded plugin


template<typename T, bool has_destructor = std::is_destructible<T>::value>
struct dtor_utils { dtor_utils() = delete; };

template<typename T>
struct dtor_utils<T, true>
{
	//C++ forbids getting the address of a dtor, but we can still wrap it
	inline static void call_dtor(void* obj) { static_cast<const T*>(obj)->~T(); }

	constexpr static dtor_t dtor = &call_dtor;
};

template<typename T>
struct dtor_utils<T, false>
{
	//Can't call a dtor that doesn't exist
	constexpr static dtor_t dtor = nullptr;
};
