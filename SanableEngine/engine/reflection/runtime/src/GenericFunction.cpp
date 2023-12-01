#include "GenericFunction.hpp"

GenericFunction::GenericFunction() :
	callConv(CallConv::Invalid)
{
}

GenericFunction::~GenericFunction()
{
}

GenericFunction::GenericFunction(const GenericFunction& cpy) :
	GenericFunction()
{
	*this = cpy;
}

GenericFunction::GenericFunction(GenericFunction&& mov) :
	GenericFunction()
{
	*this = std::move(mov);
}

GenericFunction& GenericFunction::operator=(const GenericFunction& cpy)
{
	this->returnType = cpy.returnType;
	this->args       = cpy.args;
	this->owningType = cpy.owningType;
	this->callConv   = cpy.callConv;
	return *this;
}

GenericFunction& GenericFunction::operator=(GenericFunction&& mov)
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

bool GenericFunction::isValid() const
{
	return returnType.isValid()
		&& callConv != CallConv::Invalid
		&& (owningType.isValid() == (callConv == CallConv::ThisCall));
}

GenericFunction::CallConv GenericFunction::getCallConv() const
{
	assert(isValid());
	return callConv;
}

TypeName GenericFunction::getReturnType() const
{
	assert(isValid());
	return returnType;
}

size_t GenericFunction::getArgCount() const
{
	assert(isValid());
	return args.size();
}

TypeName GenericFunction::getArg(size_t index) const
{
	assert(isValid());
	assert(index < args.size());
	return args[index];
}

bool GenericFunction::isMemberFunction() const
{
	assert(isValid());
	return owningType.isValid();
}

TypeName GenericFunction::getOwner() const
{
	assert(isValid());
	return owningType;
}
