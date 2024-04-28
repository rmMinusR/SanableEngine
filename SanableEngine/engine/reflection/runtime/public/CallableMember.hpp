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

	ParameterInfo(const TypeName& type, const std::string& name);
	~ParameterInfo();
};


class CallableMember
{
public:
	TypeName returnType;
	TypeName owner;
	std::vector<ParameterInfo> parameters;

	virtual ~CallableMember();

	ENGINE_RTTI_API void invoke(SAnyRef returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters) const;
protected:
	ENGINE_RTTI_API CallableMember(const TypeName& owner, const TypeName& returnType, const std::vector<ParameterInfo>& parameters, CallableUtils::Member::fully_erased_binder_t binder, CallableUtils::Member::erased_fp_t fn);

	//All SAnyRefs guaranteed valid when called
	CallableUtils::Member::fully_erased_binder_t binder;
	CallableUtils::Member::erased_fp_t fn;

public:
	template<typename TReturn, typename TOwner, typename... TArgs>
	static CallableMember make(TReturn(TOwner::* fn)(TArgs...))
	{
		return CallableMember(
			TypeName::create<TOwner>(),
			TypeName::create<TReturn>(),
			{ TypeName::create<TArgs>()... },
			(CallableUtils::Member::fully_erased_binder_t) &CallableUtils::Member::_impl<TReturn>::impl,
			(CallableUtils::Member::erased_fp_t) fn
		);
	}
};
