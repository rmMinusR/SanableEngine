#pragma once

#include <vector>
#include <string>
#include <functional>
#include <cassert>

#include "TypeName.hpp"
#include "SAny.hpp"
#include "CallableUtils.inl"

struct ParameterInfo
{
	TypeName type;
	std::string name;

	ENGINE_RTTI_API ParameterInfo(const TypeName& type, const std::string& name);
	ENGINE_RTTI_API ~ParameterInfo();

	//Helper for verifying if provided static type list matches dynamic type list
	template<typename It, typename TArgsHead, typename... TArgsTail>
	static void checkStaticMatchesDynamic(It it, It end)
	{
		assert(it != end);
		assert(it->type == TypeName::create<TArgsHead>());
		checkStaticMatchesDynamic<It, TArgsTail...>(it+1, end);
	}
	template<typename It> static void checkStaticMatchesDynamic(It it, It end) { assert(it == end); } //Tail case
};


class Callable
{
public:
	TypeName returnType;
	TypeName owner;
	std::vector<ParameterInfo> parameters;

	ENGINE_RTTI_API virtual ~Callable();
protected:
	Callable(TypeName returnType, TypeName owner, std::vector<ParameterInfo> parameters);
};

class CallableMember : public Callable
{
public:
	ENGINE_RTTI_API virtual ~CallableMember();
	ENGINE_RTTI_API void invoke(SAnyRef returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters) const;
	
	template<typename TReturn, typename TOwner, typename... TArgs>
	static CallableMember make(TReturn(TOwner::* fn)(TArgs...), const std::vector<ParameterInfo>& parameters)
	{
		ParameterInfo::checkStaticMatchesDynamic<std::vector<ParameterInfo>::const_iterator, TArgs...>(parameters.cbegin(), parameters.cend());
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
	ENGINE_RTTI_API CallableMember(const TypeName& owner, const TypeName& returnType, const std::vector<ParameterInfo>& parameters, CallableUtils::Member::fully_erased_binder_t binder, CallableUtils::Member::erased_fp_t fn);

	//All SAnyRefs guaranteed valid when called
	CallableUtils::Member::fully_erased_binder_t binder;
	CallableUtils::Member::erased_fp_t fn;
};

class CallableStatic : public Callable
{
public:
	ENGINE_RTTI_API virtual ~CallableStatic();
	ENGINE_RTTI_API void invoke(SAnyRef returnValue, const std::vector<SAnyRef>& parameters) const;

	template<typename TReturn, typename... TArgs>
	static CallableStatic make(TReturn(*fn)(TArgs...), const std::vector<ParameterInfo>& parameters)
	{
		ParameterInfo::checkStaticMatchesDynamic<std::vector<ParameterInfo>::const_iterator, TArgs...>(parameters.cbegin(), parameters.cend());
		auto eraser = &CallableUtils::Static::TypeEraser<TReturn>::template impl<TArgs...>;
		return CallableStatic(
			TypeName(),
			TypeName::create<TReturn>(),
			parameters,
			(CallableUtils::Static::fully_erased_binder_t) eraser,
			(CallableUtils::Static::erased_fp_t) fn
		);
	}
	
	template<typename TOwner, typename TReturn, typename... TArgs>
	static CallableStatic makeWithOwner(TReturn(*fn)(TArgs...), const std::vector<ParameterInfo>& parameters)
	{
		ParameterInfo::checkStaticMatchesDynamic<std::vector<ParameterInfo>::const_iterator, TArgs...>(parameters.cbegin(), parameters.cend());
		auto eraser = &CallableUtils::Static::TypeEraser<TReturn>::template impl<TArgs...>;
		return CallableStatic(
			TypeName::create<TOwner>(),
			TypeName::create<TReturn>(),
			parameters,
			(CallableUtils::Static::fully_erased_binder_t) eraser,
			(CallableUtils::Static::erased_fp_t) fn
		);
	}

protected:
	ENGINE_RTTI_API CallableStatic(const TypeName& owner, const TypeName& returnType, const std::vector<ParameterInfo>& parameters, CallableUtils::Static::fully_erased_binder_t binder, CallableUtils::Static::erased_fp_t fn);

	//All SAnyRefs guaranteed valid when called
	CallableUtils::Static::fully_erased_binder_t binder;
	CallableUtils::Static::erased_fp_t fn;
};
