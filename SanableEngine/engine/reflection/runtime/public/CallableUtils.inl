#pragma once

#include <cassert>
#include <vector>
#include <utility>
#include <variant>

#include "TypeName.hpp"
#include "SAny.hpp"
#include "StaticTemplateUtils.inl"


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
namespace stix::detail::CallableUtils
{
	enum class ReturnTypeGroup
	{
		Assignable,
		Void,
		Reference
	};

	template<typename T>
	constexpr ReturnTypeGroup ReturnTypeGroup_of = std::is_same_v<T, void> ? ReturnTypeGroup::Void : (std::is_reference_v<T> ? ReturnTypeGroup::Reference : ReturnTypeGroup::Assignable);

	namespace Member
	{
		struct BinderSurrogate { inline virtual void _virtual() {} };
		struct BinderSurrogateDerived : virtual BinderSurrogate { inline virtual void _virtual() override {} };
		constexpr static size_t erased_fp_target_size = std::max( sizeof(&BinderSurrogate::_virtual), sizeof(&BinderSurrogateDerived::_virtual) );
		using erased_fp_t = decltype(&BinderSurrogateDerived::_virtual);
		static_assert(erased_fp_target_size <= sizeof(erased_fp_t) && sizeof(erased_fp_t) < erased_fp_target_size+sizeof(void*));
		using fully_erased_binder_t = void(*)(erased_fp_t fn, const SAnyRef& returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters);


		template<typename TReturn, ReturnTypeGroup>
		struct TypeEraser;

		template<typename TReturn, typename TOwner, typename... TArgs>
		static void typeErasedInvoke(erased_fp_t erased_fn, const SAnyRef& returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters)
		{
			typedef TReturn(TOwner::* fn_t)(TArgs...);
			static_assert(sizeof(detail::CallableUtils::Member::erased_fp_t) >= sizeof(fn_t));
			union //reinterpret_cast doesn't allow us to do this conversion. Too bad!
			{
				fn_t _fn;
				detail::CallableUtils::Member::erased_fp_t _erased;
			} reinterpreter;
			reinterpreter._erased = erased_fn;

			TypeEraser<TReturn, ReturnTypeGroup_of<TReturn>>::__impl(reinterpreter._fn, returnValue, thisObj, parameters, std::make_index_sequence<sizeof...(TArgs)>{});
		}
		
		template<typename TReturn>
		struct TypeEraser<TReturn, ReturnTypeGroup::Assignable>
		{
			template<typename TOwner, typename... TArgs, size_t... I>
			static void __impl(TReturn(TOwner::*fn)(TArgs...), const SAnyRef& returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters, std::index_sequence<I...>)
			{
				//No need to check return type or owner type matching; this is handled in SAny::get
				
				//Check parameters match exactly
				bool good = TypeName::staticEqualsDynamic_many<std::vector<SAnyRef>::const_iterator, true, TArgs...>(parameters.begin(), parameters.end());
				assert(good);

				//Invoke
				TOwner& _this = thisObj.get<TOwner>();
				returnValue.get<TReturn>() = (_this.*fn)( std::forward<TArgs>(parameters[I].get<TArgs>()) ...);
			}
		};

		template<>
		struct TypeEraser<void, ReturnTypeGroup::Void>
		{
			template<typename TOwner, typename... TArgs, size_t... I>
			static void __impl(void(TOwner::*fn)(TArgs...), const SAnyRef& returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters, std::index_sequence<I...>)
			{
				//No need to check return type or owner type matching; this is handled in SAny::get
			
				//Check parameters match exactly
				bool good = TypeName::staticEqualsDynamic_many<std::vector<SAnyRef>::const_iterator, true, TArgs...>(parameters.begin(), parameters.end());
				assert(good);

				//Invoke
				TOwner& _this = thisObj.get<TOwner>();
				(_this.*fn)( std::forward<TArgs>(parameters[I].get<TArgs>()) ...);
			}
		};

		template<typename TReturn>
		struct TypeEraser<TReturn, ReturnTypeGroup::Reference>
		{
			template<typename TOwner, typename... TArgs, size_t... I>
			static void __impl(TReturn(TOwner::*fn)(TArgs...), const SAnyRef& returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters, std::index_sequence<I...>)
			{
				//No need to check return type or owner type matching; this is handled in SAny::get
			
				//Check parameters match exactly
				bool good = TypeName::staticEqualsDynamic_many<std::vector<SAnyRef>::const_iterator, true, TArgs...>(parameters.begin(), parameters.end());
				assert(good);

				//Invoke
				TOwner& _this = thisObj.get<TOwner>();
				returnValue.get<std::remove_reference_t<TReturn>*>() = &(_this.*fn)( std::forward<TArgs>(parameters[I].get<TArgs>()) ...);
			}
		};
	}


	namespace Static
	{
		template<typename TReturn, ReturnTypeGroup>
		struct TypeEraser;

		template<typename TReturn, typename... TArgs>
		static void typeErasedInvoke(TReturn(*fn)(TArgs...), const SAnyRef& returnValue, const std::vector<SAnyRef>& parameters)
		{
			TypeEraser<TReturn, ReturnTypeGroup_of<TReturn>>::__impl(fn, returnValue, parameters, std::make_index_sequence<sizeof...(TArgs)>{});
		}

		template<typename TReturn>
		struct TypeEraser<TReturn, ReturnTypeGroup::Assignable>
		{
			template<typename... TArgs, size_t... I>
			static void __impl(TReturn(*fn)(TArgs...), const SAnyRef& returnValue, const std::vector<SAnyRef>& parameters, std::index_sequence<I...>)
			{
				//No need to check return type or owner type matching; this is handled in SAny::get
			
				//Check parameters match exactly
				TypeName::staticEqualsDynamic_many<std::vector<SAnyRef>::const_iterator, true, TArgs...>(parameters.begin(), parameters.end());

				//Invoke
				returnValue.get<TReturn>() = (*fn)( std::forward<TArgs>(parameters[I].get<TArgs>()) ...);
			}
		};

		template<typename TReturn>
		struct TypeEraser<TReturn, ReturnTypeGroup::Void>
		{
			template<typename... TArgs, size_t... I>
			static void __impl(TReturn(*fn)(TArgs...), const SAnyRef& returnValue, const std::vector<SAnyRef>& parameters, std::index_sequence<I...>)
			{
				//No need to check return type or owner type matching; this is handled in SAny::get
			
				//Check parameters match exactly
				TypeName::staticEqualsDynamic_many<std::vector<SAnyRef>::const_iterator, true, TArgs...>(parameters.begin(), parameters.end());

				//Invoke
				(*fn)( std::forward<TArgs>(parameters[I].get<TArgs>()) ...);
			}
		};

		template<typename TReturn>
		struct TypeEraser<TReturn, ReturnTypeGroup::Reference>
		{
			template<typename... TArgs, size_t... I>
			static void __impl(TReturn(*fn)(TArgs...), const SAnyRef& returnValue, const std::vector<SAnyRef>& parameters, std::index_sequence<I...>)
			{
				//No need to check return type or owner type matching; this is handled in SAny::get
			
				//Check parameters match exactly
				TypeName::staticEqualsDynamic_many<std::vector<SAnyRef>::const_iterator, true, TArgs...>(parameters.begin(), parameters.end());

				//Invoke
				returnValue.get<std::remove_reference_t<TReturn>*>() = &(*fn)( std::forward<TArgs>(parameters[I].get<TArgs>()) ...);
			}
		};


		template<typename TReturn, typename... TArgs>
		using binder_t = void(*)(TReturn(*fn)(TArgs...), const SAnyRef& returnValue, const std::vector<SAnyRef>& parameters);

		using fully_erased_binder_t = binder_t<void>;

		typedef void(*erased_fp_t)();
	}
}
