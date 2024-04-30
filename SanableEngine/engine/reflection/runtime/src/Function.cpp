#include "Function.hpp"

#include <cassert>

#include "TypeInfo.hpp"

stix::Function::Function(const TypeName& returnType, const std::vector<TypeName>& parameters) :
	returnType(returnType),
	parameters(parameters)
{
}

stix::Function::~Function()
{
}

stix::MemberFunction::MemberFunction(const TypeName& owner, bool ownerIsConst, const TypeName& returnType, const std::vector<TypeName>& parameters,
	                                 detail::CallableUtils::Member::fully_erased_binder_t binder, detail::CallableUtils::Member::erased_fp_t fn) :
	Function(returnType, parameters),
	owner(owner),
	ownerIsConst(ownerIsConst),
	binder(binder),
	fn(fn)
{
}

stix::MemberFunction::~MemberFunction()
{
}

#if WIN32
#define STACK_ALLOC _malloca
#else
#define STACK_ALLOC alloca
#endif

void stix::MemberFunction::invoke(SAnyRef returnValue, const SAnyRef& thisObj, const std::vector<SAnyRef>& parameters) const
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

stix::StaticFunction::StaticFunction(const TypeName& returnType, const std::vector<TypeName>& parameters,
	                                 detail::CallableUtils::Static::fully_erased_binder_t binder, detail::CallableUtils::Static::erased_fp_t fn) :
	Function(returnType, parameters),
	binder(binder),
	fn(fn)
{
}

stix::StaticFunction::~StaticFunction()
{
}

void stix::StaticFunction::invoke(SAnyRef returnValue, const std::vector<SAnyRef>& parameters) const
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
