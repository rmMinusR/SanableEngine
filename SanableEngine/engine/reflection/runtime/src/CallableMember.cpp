#include "CallableMember.hpp"

#include <cassert>

#include "TypeInfo.hpp"

ParameterInfo::ParameterInfo(const TypeName& type, const std::string& name) :
	type(type),
	name(name)
{
}

ParameterInfo::~ParameterInfo()
{
}

CallableMember::CallableMember(const TypeName& owner, const TypeName& returnType, const std::vector<ParameterInfo>& parameters, CallableUtils::Member::fully_erased_binder_t binder, void(CallableUtils::Member::BinderSurrogate::*fn)()) :
	owner(owner),
	returnType(returnType),
	parameters(parameters),
	binder(binder),
	fn(fn)
{
}

CallableMember::~CallableMember()
{
}

#if WIN32
#define STACK_ALLOC _malloca
#define STACK_FREE
#else
#define STACK_ALLOC alloca
#endif

void CallableMember::invoke(SAnyRef returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters) const
{
	assert(thisObj);

	if (!returnValue && returnType != TypeName::create<void>())
	{
		//Create a temporary
		size_t returnValSize = returnType.resolve()->size;
		void* tempReturnVal = STACK_ALLOC(returnValSize);
		memset(tempReturnVal, 0, returnValSize);
		returnValue = SAnyRef(tempReturnVal, returnType);
	}

	//Invoke
	binder(fn, returnValue, thisObj, parameters);
}
