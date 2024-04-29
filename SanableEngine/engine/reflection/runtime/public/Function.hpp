#pragma once

#include <vector>
#include <string>
#include <functional>
#include <cassert>

#include "TypeName.hpp"
#include "SAny.hpp"
#include "CallableUtils.inl"

namespace stix
{

	class Function
	{
	public:
		TypeName returnType;
		std::vector<TypeName> parameters;

		ENGINE_RTTI_API virtual ~Function();
	protected:
		Function(const TypeName& returnType, const std::vector<TypeName>& parameters);
	};

	class MemberFunction : public Function
	{
	public:
		ENGINE_RTTI_API virtual ~MemberFunction();
		ENGINE_RTTI_API void invoke(SAnyRef returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters) const;
	
		template<typename TReturn, typename TOwner, typename... TArgs>
		static MemberFunction make(TReturn(TOwner::* fn)(TArgs...))
		{
			std::vector<TypeName> parameters = TypeName::createPack<TArgs...>();
			detail::CallableUtils::checkArgs<std::vector<TypeName>::const_iterator, TArgs...>(parameters.cbegin(), parameters.cend());
			auto eraser = &detail::CallableUtils::Member::TypeEraser<TReturn>::template impl<TOwner, TArgs...>;
			return MemberFunction(
				TypeName::create<TOwner>(),
				TypeName::create<TReturn>(),
				parameters,
				(detail::CallableUtils::Member::fully_erased_binder_t) eraser,
				(detail::CallableUtils::Member::erased_fp_t) fn
			);
		}

	protected:
		ENGINE_RTTI_API MemberFunction(const TypeName& owner, const TypeName& returnType, const std::vector<TypeName>& parameters, detail::CallableUtils::Member::fully_erased_binder_t binder, detail::CallableUtils::Member::erased_fp_t fn);

		//All SAnyRefs guaranteed valid when called
		TypeName owner;
		detail::CallableUtils::Member::fully_erased_binder_t binder;
		detail::CallableUtils::Member::erased_fp_t fn;
	};

	class StaticFunction : public Function
	{
	public:
		ENGINE_RTTI_API virtual ~StaticFunction();
		ENGINE_RTTI_API void invoke(SAnyRef returnValue, const std::vector<SAnyRef>& parameters) const;

		template<typename TReturn, typename... TArgs>
		static StaticFunction make(TReturn(*fn)(TArgs...))
		{
			std::vector<TypeName> parameters = TypeName::createPack<TArgs...>();
			detail::CallableUtils::checkArgs<std::vector<TypeName>::const_iterator, TArgs...>(parameters.cbegin(), parameters.cend());
			auto eraser = &detail::CallableUtils::Static::TypeEraser<TReturn>::template impl<TArgs...>;
			return StaticFunction(
				TypeName::create<TReturn>(),
				parameters,
				(detail::CallableUtils::Static::fully_erased_binder_t) eraser,
				(detail::CallableUtils::Static::erased_fp_t) fn
			);
		}
	
	protected:
		ENGINE_RTTI_API StaticFunction(const TypeName& returnType, const std::vector<TypeName>& parameters, detail::CallableUtils::Static::fully_erased_binder_t binder, detail::CallableUtils::Static::erased_fp_t fn);

		//All SAnyRefs guaranteed valid when called
		detail::CallableUtils::Static::fully_erased_binder_t binder;
		detail::CallableUtils::Static::erased_fp_t fn;
	};

}
