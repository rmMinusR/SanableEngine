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
	};

	template<typename Secret, auto M>
	struct _access_giver
	{
		static const inline auto m = _caster<decltype(M), Secret>::m = M;
	};

	#define PUBLIC_CAST_KEY(TClass, name) __PUBLIC_CAST_KEY__##TClass##__##name
	#define DO_PUBLIC_CAST_RAW(TClass, memberType, name) ::public_cast::_caster<memberType, PUBLIC_CAST_KEY(TClass, name)>::m

	#define DO_PUBLIC_CAST(TClass, memberType, name) DO_PUBLIC_CAST_RAW(TClass, memberType TClass::*, name)
	#define DO_PUBLIC_CAST_MEMFN(TClass, returnType, args, name) DO_PUBLIC_CAST(TClass, ::public_cast::_mem_fns<TClass, returnType, args>::ptr_t, name)

	//Source file only
	#define PUBLIC_CAST_GIVE_ACCESS(TClass, name) \
		struct PUBLIC_CAST_KEY(TClass, name) {};\
		template struct ::public_cast::_access_giver<PUBLIC_CAST_KEY(TClass, name), &TClass::name>
}
