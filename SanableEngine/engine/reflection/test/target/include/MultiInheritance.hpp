#pragma once

struct IFoo
{
	virtual int foo() = 0;
};

struct IBar
{
	virtual int bar() = 0;
};

struct ConcreteBase
{
	int a;
	virtual ~ConcreteBase();
};

struct CommonInterface : public ConcreteBase, public IFoo, public IBar
{

};

struct ImplementerA : public CommonInterface
{
	ImplementerA();

	virtual int foo() override;
	virtual int bar() override;
};

struct ImplementerB : public CommonInterface
{
	ImplementerB();

	virtual int foo() override;
	virtual int bar() override;
};