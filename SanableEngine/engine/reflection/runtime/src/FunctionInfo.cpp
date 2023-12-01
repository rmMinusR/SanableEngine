#include "FunctionInfo.hpp"

FunctionInfo::FunctionInfo() :
	callConv(CallConv::Invalid)
{
}

FunctionInfo::~FunctionInfo()
{
}

FunctionInfo::FunctionInfo(const FunctionInfo& cpy) :
	FunctionInfo()
{
	*this = cpy;
}

FunctionInfo::FunctionInfo(FunctionInfo&& mov) :
	FunctionInfo()
{
	*this = std::move(mov);
}

FunctionInfo& FunctionInfo::operator=(const FunctionInfo& cpy)
{
	this->returnType = cpy.returnType;
	this->args       = cpy.args;
	this->owningType = cpy.owningType;
	this->callConv   = cpy.callConv;
	return *this;
}

FunctionInfo& FunctionInfo::operator=(FunctionInfo&& mov)
{
	this->returnType = std::move(mov.returnType);
	this->args       = std::move(mov.args      );
	this->owningType = std::move(mov.owningType);
	this->callConv   = std::move(mov.callConv  );

	mov.returnType = TypeName();
	mov.args.clear();
	mov.owningType = TypeName();
	mov.callConv = CallConv::Invalid;

	return *this;
}

bool FunctionInfo::isValid() const
{
	return returnType.isValid()
		&& callConv != CallConv::Invalid
		&& (owningType.isValid() == (callConv == CallConv::ThisCall));
}

FunctionInfo::CallConv FunctionInfo::getCallConv() const
{
	assert(isValid());
	return callConv;
}

TypeName FunctionInfo::getReturnType() const
{
	assert(isValid());
	return returnType;
}

size_t FunctionInfo::getArgCount() const
{
	assert(isValid());
	return args.size();
}

TypeName FunctionInfo::getArg(size_t index) const
{
	assert(isValid());
	assert(index < args.size());
	return args[index];
}

bool FunctionInfo::isMemberFunction() const
{
	assert(isValid());
	return owningType.isValid();
}

TypeName FunctionInfo::getOwner() const
{
	assert(isValid());
	return owningType;
}
