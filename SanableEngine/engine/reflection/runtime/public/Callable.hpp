#pragma once

#include <vector>
#include <string>
#include <functional>
#include <cassert>

#include "TypeName.hpp"
#include "SAny.hpp"
#include "CallableUtils.inl"

class Callable
{
public:
	TypeName returnType;
	std::vector<TypeName> parameters;

	ENGINE_RTTI_API virtual ~Callable();
protected:
	Callable(const TypeName& returnType, const std::vector<TypeName>& parameters);
};

class CallableMember : public Callable
{
public:
	ENGINE_RTTI_API virtual ~CallableMember();
	ENGINE_RTTI_API void invoke(SAnyRef returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters) const;
	
	template<typename TReturn, typename TOwner, typename... TArgs>
	static CallableMember make(TReturn(TOwner::* fn)(TArgs...))
	{
		std::vector<TypeName> parameters = TypeName::createPack<TArgs...>();
		CallableUtils::checkArgs<std::vector<TypeName>::const_iterator, TArgs...>(parameters.cbegin(), parameters.cend());
		auto eraser = &CallableUtils::Member::TypeEraser<TReturn>::template impl<TOwner, TArgs...>;
		return CallableMember(
			TypeName::create<TOwner>(),
			TypeName::create<TReturn>(),
			parameters,
			(CallableUtils::Member::fully_erased_binder_t) eraser,
			(CallableUtils::Member::erased_fp_t) fn
		);
	}

protected:
	ENGINE_RTTI_API CallableMember(const TypeName& owner, const TypeName& returnType, const std::vector<TypeName>& parameters, CallableUtils::Member::fully_erased_binder_t binder, CallableUtils::Member::erased_fp_t fn);

	//All SAnyRefs guaranteed valid when called
	TypeName owner;
	CallableUtils::Member::fully_erased_binder_t binder;
	CallableUtils::Member::erased_fp_t fn;
};

class CallableStatic : public Callable
{
public:
	ENGINE_RTTI_API virtual ~CallableStatic();
	ENGINE_RTTI_API void invoke(SAnyRef returnValue, const std::vector<SAnyRef>& parameters) const;

	template<typename TReturn, typename... TArgs>
	static CallableStatic make(TReturn(*fn)(TArgs...))
	{
		std::vector<TypeName> parameters = TypeName::createPack<TArgs...>();
		CallableUtils::checkArgs<std::vector<TypeName>::const_iterator, TArgs...>(parameters.cbegin(), parameters.cend());
		auto eraser = &CallableUtils::Static::TypeEraser<TReturn>::template impl<TArgs...>;
		return CallableStatic(
			TypeName::create<TReturn>(),
			parameters,
			(CallableUtils::Static::fully_erased_binder_t) eraser,
			(CallableUtils::Static::erased_fp_t) fn
		);
	}
	
protected:
	ENGINE_RTTI_API CallableStatic(const TypeName& returnType, const std::vector<TypeName>& parameters, CallableUtils::Static::fully_erased_binder_t binder, CallableUtils::Static::erased_fp_t fn);

	//All SAnyRefs guaranteed valid when called
	CallableUtils::Static::fully_erased_binder_t binder;
	CallableUtils::Static::erased_fp_t fn;
};
