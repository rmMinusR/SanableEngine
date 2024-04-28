#pragma once

#include <cassert>
#include <vector>

#include "TypeName.hpp"
#include "SAny.hpp"

//Ugly templated type erasure utils for binding concrete functions to be callable with SAnys

namespace CallableUtils
{
	namespace Member
	{
		template<typename TReturn, bool returnsVoid = std::is_same_v<TReturn, void>>
		struct _impl;

		template<typename TReturn>
		struct _impl<TReturn, false>
		{
			template<typename TOwner, typename... TArgs>
			static void impl(TReturn(TOwner::*fn)(TArgs...), const SAnyRef& returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters)
			{
				//No need to check return type or owner type matching; this is handled in SAny::get
			
				//Check parameters match exactly
				assert(parameters.size() == sizeof...(TArgs));
				typedef std::make_index_sequence<sizeof...(TArgs)> ArgIDs;
				assert(*parameters[ArgIDs].getType() == TypeName::create<TArgs>() && ...);

				*returnValue.get<TReturn>() = thisObj.get<TOwner>()->fn( std::forward(*parameters[ArgIDs].get<TArgs>()) ...);
			}
		};

		template<typename TReturn>
		struct _impl<TReturn, true>
		{
			template<typename TOwner, typename... TArgs>
			static void impl(TReturn(TOwner::* fn)(TArgs...), const SAnyRef& returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters)
			{
				//No need to check return type or owner type matching; this is handled in SAny::get

				//Check parameters match exactly
				assert(parameters.size() == sizeof...(TArgs));
				typedef std::make_index_sequence<sizeof...(TArgs)> ArgIDs;
				assert(*parameters[ArgIDs].getType() == TypeName::create<TArgs>() && ...);

				thisObj.get<TOwner>()->fn( std::forward(*parameters[ArgIDs].get<TArgs>()) ...);
			}
		};


		template<typename TReturn, typename TOwner, typename... TArgs>
		using binder_t = void(*)(TReturn(TOwner::* fn)(TArgs...), const SAnyRef& returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters);

		struct BinderSurrogate {};
		using fully_erased_binder_t = binder_t<void, BinderSurrogate>;

		typedef void(BinderSurrogate::* erased_fp_t)();
	}
}
