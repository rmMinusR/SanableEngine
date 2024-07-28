#pragma once

void globalFunc(int a, char b, const void* c);

class MyClass
{
	MyClass();
	virtual ~MyClass();

	MyClass(const MyClass& cpy) = delete;

	int foo;
	char bar;

	void myClassFunc(int param);
	const int* myConstClassFunc(int param) const;
	static void myStaticClassFunc(int param, MyClass* cls);
};

class NonDefaulted
{
	NonDefaulted(int foo);
};

namespace MyNamespace
{
	void globalFuncInNamespace(int a, char b, const void* c);
	class ClassInNamespace {};
}