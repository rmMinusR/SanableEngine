#include "MultiInheritance.hpp"

ConcreteBase::~ConcreteBase()
{
}

ImplementerA::ImplementerA()
{
	a = 1;
}

int ImplementerA::foo()
{
	return 1;
}

int ImplementerA::bar()
{
	return 2;
}

ImplementerB::ImplementerB()
{
	a = 2;
}

int ImplementerB::foo()
{
	return 3;
}

int ImplementerB::bar()
{
	return 4;
}
