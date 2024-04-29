#pragma once

#include <cassert>
#include <vector>
#include <utility>

#include "TypeName.hpp"
#include "SAny.hpp"


namespace FuncPtrAliases
{
	template<typename TReturn, typename... TArgs>
	struct Static { typedef TReturn(*ptr_t)(TArgs...); };

	template<typename TReturn, typename TOwner, typename... TArgs>
	struct Member
	{
		typedef TReturn(TOwner::*normal_t)(TArgs...);
		typedef TReturn(TOwner::*const_t)(TArgs...) const;
	};
}

//Ugly templated type erasure utils for binding concrete functions to be callable with SAnys
namespace CallableUtils
{
	//Helper for verifying if provided static type list matches dynamic type list
	template<typename It, typename TArgsHead, typename... TArgsTail>
	static void checkArgs(It it, It end)
	{
		assert(it != end);
		assert(it->getType() == TypeName::create<TArgsHead>());
		checkArgs<It, TArgsTail...>(it+1, end);
	}
	template<typename It> static void checkArgs(It it, It end) { assert(it == end); } //Tail case
	

	namespace Member
	{
		template<typename TReturn, bool returnsVoid = std::is_same_v<TReturn, void>>
		struct TypeEraser;

		template<typename TReturn>
		struct TypeEraser<TReturn, false>
		{
			template<typename TOwner, typename... TArgs>
			static void impl(TReturn(TOwner::*fn)(TArgs...), const SAnyRef& returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters)
			{
				__impl(fn, returnValue, thisObj, parameters, std::make_index_sequence<sizeof...(TArgs)>{});
			}
			
		private:
			template<typename TOwner, typename... TArgs, size_t... I>
			static void __impl(TReturn(TOwner::*fn)(TArgs...), const SAnyRef& returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters, std::index_sequence<I...>)
			{
				//No need to check return type or owner type matching; this is handled in SAny::get
			
				//Check parameters match exactly
				checkArgs<std::vector<SAnyRef>::const_iterator, TArgs...>(parameters.begin(), parameters.end());

				//Invoke
				TOwner& _this = thisObj.get<TOwner>();
				returnValue.get<TReturn>() = (_this.*fn)( std::forward<TArgs>(parameters[I].get<TArgs>()) ...);
			}
		};

		template<typename TReturn>
		struct TypeEraser<TReturn, true>
		{
			template<typename TOwner, typename... TArgs>
			static void impl(TReturn(TOwner::*fn)(TArgs...), const SAnyRef& returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters)
			{
				__impl(fn, returnValue, thisObj, parameters, std::make_index_sequence<sizeof...(TArgs)>{});
			}
			
		private:
			template<typename TOwner, typename... TArgs, size_t... I>
			static void __impl(TReturn(TOwner::*fn)(TArgs...), const SAnyRef& returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters, std::index_sequence<I...>)
			{
				//No need to check return type or owner type matching; this is handled in SAny::get
			
				//Check parameters match exactly
				checkArgs<std::vector<SAnyRef>::const_iterator, TArgs...>(parameters.begin(), parameters.end());

				//Invoke
				TOwner& _this = thisObj.get<TOwner>();
				(_this.*fn)( std::forward<TArgs>(parameters[I].get<TArgs>()) ...);
			}
		};


		template<typename TReturn, typename TOwner, typename... TArgs>
		using binder_t = void(*)(TReturn(TOwner::* fn)(TArgs...), const SAnyRef& returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters);

		struct BinderSurrogate {};
		using fully_erased_binder_t = binder_t<void, BinderSurrogate>;

		typedef void(BinderSurrogate::* erased_fp_t)();
	}
}
