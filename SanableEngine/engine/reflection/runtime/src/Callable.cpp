#include "Callable.hpp"

#include <cassert>

#include "TypeInfo.hpp"

Callable::Callable(const TypeName& returnType, const std::vector<TypeName>& parameters) :
	returnType(returnType),
	parameters(parameters)
{
}

Callable::~Callable()
{
}

CallableMember::CallableMember(const TypeName& owner, const TypeName& returnType, const std::vector<TypeName>& parameters, CallableUtils::Member::fully_erased_binder_t binder, void(CallableUtils::Member::BinderSurrogate::*fn)()) :
	Callable(returnType, parameters),
	owner(owner),
	binder(binder),
	fn(fn)
{
}

CallableMember::~CallableMember()
{
}

#if WIN32
#define STACK_ALLOC _malloca
#else
#define STACK_ALLOC alloca
#endif

void CallableMember::invoke(SAnyRef returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters) const
{
	assert(thisObj);

	bool returnsVoid = returnType==TypeName::create<void>();
	if (!returnValue && !returnsVoid)
	{
		//Create a temporary
		size_t returnValSize = returnType.resolve()->layout.size;
		void* tempReturnVal = STACK_ALLOC(returnValSize);
		memset(tempReturnVal, 0, returnValSize);
		returnValue = SAnyRef(tempReturnVal, returnType);
	}
	else if (returnsVoid) assert(!returnValue);

	//Invoke
	binder(fn, returnValue, thisObj, parameters); //This will implicitly reinterpret fn to the right type when we enter the binder function itself
}

CallableStatic::CallableStatic(const TypeName& returnType, const std::vector<TypeName>& parameters, CallableUtils::Static::fully_erased_binder_t binder, CallableUtils::Static::erased_fp_t fn) :
	Callable(returnType, parameters),
	binder(binder),
	fn(fn)
{
}

CallableStatic::~CallableStatic()
{
}

void CallableStatic::invoke(SAnyRef returnValue, const std::vector<SAnyRef>& parameters) const
{
	bool returnsVoid = returnType==TypeName::create<void>();
	if (!returnValue && !returnsVoid)
	{
		//Create a temporary
		size_t returnValSize = returnType.resolve()->layout.size;
		void* tempReturnVal = STACK_ALLOC(returnValSize);
		memset(tempReturnVal, 0, returnValSize);
		returnValue = SAnyRef(tempReturnVal, returnType);
	}
	else if (returnsVoid) assert(!returnValue);

	//Invoke
	binder(fn, returnValue, parameters); //This will implicitly reinterpret fn to the right type when we enter the binder function itself
}
