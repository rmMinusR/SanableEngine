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
// PUBLIC_CAST_GIVE_ACCESS(MyType, priv);
// 
// Then, from anywhere later:
// myObject.*DO_PUBLIC_CAST(MyType, int, priv) = 2;
// 

namespace public_cast
{
	template<class M, typename Secret>
	struct _caster
	{
		static inline M m{};
		//_type_lut<M>::t = decltype(M);
	};

	template<typename Secret, auto M>
	struct _access_giver
	{
		static const inline auto m = _caster<decltype(M), Secret>::m = M;
	};

	template<typename Secret>
	struct _type_lut {};
	
	#define PUBLIC_CAST_KEY(TClass, name) __PUBLIC_CAST_KEY__##TClass##__##name
	#define DO_PUBLIC_CAST(TClass, name) ::public_cast::_caster<::public_cast::_type_lut<PUBLIC_CAST_KEY(TClass, name)>::ptr_t, PUBLIC_CAST_KEY(TClass, name)>::m

	//Source file only
	#define PUBLIC_CAST_GIVE_ACCESS(TClass, memberTypePtr, name) \
		struct PUBLIC_CAST_KEY(TClass, name) {}; \
		template struct ::public_cast::_access_giver<PUBLIC_CAST_KEY(TClass, name), &TClass::name>; \
		template<> struct ::public_cast::_type_lut<PUBLIC_CAST_KEY(TClass, name)> { using ptr_t = memberTypePtr; };

	#define PUBLIC_CAST_PTR_TO_FIELD(TClass, memberType) memberType TClass::*
	#define PUBLIC_CAST_PTR_TO_STATIC_VAR(TClass, memberType) memberType*
	#define PUBLIC_CAST_PTR_TO_BOUND_FN(TClass, returnType, /*args*/...) returnType (TClass::*)(__VA_ARGS__)
	#define PUBLIC_CAST_PTR_TO_STATIC_FN(TClass, returnType, /*args*/...) returnType (*)(__VA_ARGS__)
}
