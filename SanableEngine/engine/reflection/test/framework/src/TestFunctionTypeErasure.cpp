#include <doctest/doctest.h>

#include "CallableMember.hpp"

struct MyCallable
{
	int canary;

	inline void incCanary() { ++canary; }
	inline int incCanary2() { return ++canary; }

	inline void setCanary(int val) { canary = val; }
	inline int sum(int a, int b) { return a + b; }


	MyCallable* ptrCanary;
	inline void passStructPtr(MyCallable* v) { ptrCanary =  v; }
	inline void passStructRef(MyCallable& v) { ptrCanary = &v; }
	inline MyCallable* passthroughStructPtr(MyCallable* v) { return ptrCanary = v; }
	inline MyCallable& passthroughStructRef(MyCallable& v) { return *(ptrCanary = &v); }
	inline MyCallable* returnStructPtr(MyCallable* v) { return  ptrCanary; }
	inline MyCallable& returnStructRef(MyCallable& v) { return *ptrCanary; }
};

TEST_CASE("Function type erasure")
{
	SUBCASE("Member")
	{
		SUBCASE("Primitive types")
		{
			SUBCASE("void()")
			{
				CallableMember fn = CallableMember::make(&MyCallable::incCanary);

				MyCallable obj;
				obj.canary = 0;
				fn.invoke(SAnyRef(), SAnyRef::make(&obj), {});

				CHECK(obj.canary == 1);
			}

			SUBCASE("int()")
			{
				CallableMember fn = CallableMember::make(&MyCallable::incCanary2);

				MyCallable obj;
				obj.canary = 0;
				int result;
				fn.invoke(SAnyRef::make(&result), SAnyRef::make(&obj), {});

				CHECK(obj.canary == 1);
				CHECK(result == 1);
			}

			SUBCASE("void(int)")
			{
				CallableMember fn = CallableMember::make(&MyCallable::setCanary);

				MyCallable obj;
				obj.canary = 0;
				int val = 20;
				fn.invoke(SAnyRef(), SAnyRef::make(&obj), { SAnyRef::make(&val) });

				CHECK(obj.canary == val);
			}

			SUBCASE("int(int)")
			{
				CallableMember fn = CallableMember::make(&MyCallable::sum);

				MyCallable obj;
				int result;
				int a = 2;
				int b = 3;
				fn.invoke(SAnyRef::make(&result), SAnyRef::make(&obj), { SAnyRef::make(&a), SAnyRef::make(&b) });

				CHECK(result == a+b);
			}
		}

		//TODO
		
		/*
		SUBCASE("Pointers and references")
		{
			SUBCASE("void(struct*)") {}
			SUBCASE("void(struct&)") {}
			SUBCASE("struct*(struct*)") {}
			SUBCASE("struct&(struct&)") {}
			SUBCASE("struct*()") {}
			SUBCASE("struct&()") {}
		}

		SUBCASE("Const")
		{
			SUBCASE("void(const struct*)") {}
			SUBCASE("void(const struct&)") {}
			SUBCASE("void(const struct)") {}
			SUBCASE("const struct*()") {}
			SUBCASE("const struct&()") {}
			SUBCASE("const struct()") {}
		}

		SUBCASE("Raw structs")
		{
			SUBCASE("void(struct)") {}
			SUBCASE("struct(struct)") {}
			SUBCASE("struct()") {}
		}
		// */
	}
}
