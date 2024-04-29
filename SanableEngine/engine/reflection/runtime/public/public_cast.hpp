#pragma once

// 
// BE WARNED, ALL YE WHO INCLUDE THIS FILE:
//
// The functions below make all private members visible. Needless to say, this
// is a crime against good code, and probably your sanity as well. Nonetheless,
// it may still be necessary under some rare circumstances.
// 
// For example, Sanable uses this to generate reflection data without putting
// macros everywhere, which would be intrusive and against the philosophy.
// 
// The ideas behind this come from Logan Smith's Cursed Casts:
// https://youtu.be/SmlLdd1Q2V8?si=YM62dsFDbui-A4AG&t=698
// And this implementation is heavily based on his.
// 
// DO NOT USE IF YOU CAN AVOID IT.
// 


// 
// To use, do this once in a source file:
// PUBLIC_CAST_GIVE_FIELD_ACCESS(MyType_privateField, MyType, privateField, int);
// Or similar for static variables, or member/static functions
// 
// Then, from anywhere later:
// myObject.*DO_PUBLIC_CAST(MyType, privateField) = 2;
// 
// You can also get something similar to the offsetof macro using:
// DO_PUBLIC_CAST_OFFSETOF_LAMBDA(MyType, privateField)
// Which takes an instance of the object, and returns a ptrdiff_t.
// Unless you are using virtual inheritance, you can pass nullptr as the instance.
// 

#include <type_traits>

namespace public_cast
{
	#pragma region Backend detail macros

	template<typename Secret>
	struct _type_lut { };

	template<class M, typename Secret>
	struct _caster
	{
		static inline M m{};
	};

	template<typename Secret, auto M>
	struct _access_giver
	{
		static const inline auto m = _caster<decltype(M), Secret>::m = M;
		static_assert(std::is_same_v<typename _type_lut<Secret>::ptr_t, decltype(M)>); //Safety check: Prevent declarations of mismatched type
	};

	#pragma endregion

	//Where accessing myObject->myField, the following is equivalent: myObject->*DO_PUBLIC_CAST(keyToMyField)
	//This is fully equivalent, acting as both lvalue and rvalue, and will be callable for functions
	//For statics, just dereference the pointer not as member: *DO_PUBLIC_CAST(keyToMyStatic)
	#define DO_PUBLIC_CAST(key) ::public_cast::_caster<::public_cast::_type_lut<__PUBLIC_CAST_KEY__##key>::ptr_t, __PUBLIC_CAST_KEY__##key>::m
	#define DO_PUBLIC_CAST_OFFSETOF_LAMBDA(key, TClass) \
		[](const void* obj) {\
			auto fieldLoc = std::addressof( ((const TClass*)obj)->*DO_PUBLIC_CAST(key) );\
			return ( (const char*)fieldLoc - (const char*)obj );\
		}
	//Obsolete: Doesn't work on virtually-inherited types
	//#define DO_PUBLIC_CAST_OFFSETOF(key, TClass) ((char*)std::addressof(((TClass*)nullptr)->*DO_PUBLIC_CAST(key)) - (char*)nullptr)
	

	//Place these in a source file (NOT header) to grant access to a private member. Must be written before DO_PUBLIC_CAST uses them.
	//These are type safe, but for the sake of convenience you will probably want to use the rttigen.py script to generate these.
	
	#pragma region Bare setup macros

	//Lets you go lower-level than the convenience functions
	//Most programs won't need this unless they're dealing with templates

	#define PUBLIC_CAST_KEY_OF(key) __PUBLIC_CAST_KEY__##key
	#define PUBLIC_CAST_DECLARE_KEY_BARE(key) struct PUBLIC_CAST_KEY_OF(key) {}
	#define PUBLIC_CAST_GIVE_ACCESS_BARE(key, TClass, name) \
		template struct ::public_cast::_access_giver<__PUBLIC_CAST_KEY__##key, &TClass::name>

	#pragma endregion

	#pragma region Convenience setup macros

	//NOTE: memberTypePtr must be passed as __VA_ARGS__ since the C preprocessor will mess up templates otherwise
	#define PUBLIC_CAST_GIVE_FIELD_ACCESS(key, TClass, name, /*declared type*/...) \
		PUBLIC_CAST_DECLARE_KEY_BARE(key); \
		template<> struct ::public_cast::_type_lut<PUBLIC_CAST_KEY_OF(key)> { using owner_t = TClass; using decl_t = __VA_ARGS__; using ptr_t = decl_t owner_t::*; }; \
		PUBLIC_CAST_GIVE_ACCESS_BARE(key, TClass, name);

	#define PUBLIC_CAST_GIVE_STATIC_VAR_ACCESS(key, TClass, name, /*declared type*/...) \
		PUBLIC_CAST_DECLARE_KEY_BARE(key); \
		template<> struct ::public_cast::_type_lut<PUBLIC_CAST_KEY_OF(key)> { using decl_t = __VA_ARGS__; using ptr_t = decl_t*; }; \
		PUBLIC_CAST_GIVE_ACCESS_BARE(key, TClass, name);

	//NOTE: If return type is a template with commas, you will need to write this by hand or the C preprocessor will mangle it
	#define PUBLIC_CAST_GIVE_BOUND_FN_ACCESS(key, TClass, name, returnType, /*args*/...) \
		PUBLIC_CAST_DECLARE_KEY_BARE(key); \
		template<> struct ::public_cast::_type_lut<PUBLIC_CAST_KEY_OF(key)> { using ptr_t = returnType (TClass::*)(__VA_ARGS__); }; \
		PUBLIC_CAST_GIVE_ACCESS_BARE(key, TClass, name);

	//NOTE: If return type is a template with commas, you will need to write this by hand or the C preprocessor will mangle it
	#define PUBLIC_CAST_GIVE_STATIC_FN_ACCESS(key, TClass, name, returnType, /*args*/...) \
		PUBLIC_CAST_DECLARE_KEY_BARE(key); \
		template<> struct ::public_cast::_type_lut<PUBLIC_CAST_KEY_OF(key)> { using ptr_t = returnType (*)(__VA_ARGS__); }; \
		PUBLIC_CAST_GIVE_ACCESS_BARE(key, TClass, name);

	#pragma endregion
}
