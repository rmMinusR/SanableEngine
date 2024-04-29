#include <doctest/doctest.h>

#include "CallableMember.hpp"

struct MyCallable
{
	char padding[64];

	int canary;
	inline MyCallable(int canaryVal = 0) : canary(canaryVal) {}

	inline void incCanary() { ++canary; }
	inline int incCanary2() { return ++canary; }

	inline void setCanary(int val) { canary = val; }
	inline int sum(int a, int b) { return a + b; }


	MyCallable* ptrCanary;
	inline void passStructPtr(MyCallable* v) { ptrCanary =  v; }
	inline void passStructRef(MyCallable& v) { ptrCanary = &v; }
	inline MyCallable* passthroughStructPtr(MyCallable* v) { return ptrCanary = v; }
	inline MyCallable& passthroughStructRef(MyCallable& v) { return *(ptrCanary = &v); }
	inline MyCallable* returnStructPtr() { return  ptrCanary; }
	inline MyCallable& returnStructRef() { return *ptrCanary; }


	inline void passStruct_copyCanary(MyCallable v) { this->canary = v.canary; }
	inline MyCallable returnStruct() { MyCallable v; v.canary = 123; return v; }
};

TEST_CASE("Function type erasure")
{
	SUBCASE("Member")
	{
		SUBCASE("Primitive types")
		{
			SUBCASE("void()")
			{
				CallableMember fn = CallableMember::make(&MyCallable::incCanary, {});

				MyCallable obj;
				obj.canary = 0;
				fn.invoke(SAnyRef(), SAnyRef::make(&obj), {});

				CHECK(obj.canary == 1);
			}

			SUBCASE("int()")
			{
				CallableMember fn = CallableMember::make(&MyCallable::incCanary2, {});

				MyCallable obj;
				obj.canary = 0;
				int result;
				fn.invoke(SAnyRef::make(&result), SAnyRef::make(&obj), {});

				CHECK(obj.canary == 1);
				CHECK(result == 1);
			}

			SUBCASE("void(int)")
			{
				CallableMember fn = CallableMember::make(&MyCallable::setCanary, { ParameterInfo(TypeName::create<int>(), "v") });

				MyCallable obj;
				obj.canary = 0;
				int val = 20;
				fn.invoke(SAnyRef(), SAnyRef::make(&obj), { SAnyRef::make(&val) });

				CHECK(obj.canary == val);
			}

			SUBCASE("int(int)")
			{
				CallableMember fn = CallableMember::make(&MyCallable::sum, { ParameterInfo(TypeName::create<int>(), "a"), ParameterInfo(TypeName::create<int>(), "b") });

				MyCallable obj;
				int result;
				int a = 2;
				int b = 3;
				fn.invoke(SAnyRef::make(&result), SAnyRef::make(&obj), { SAnyRef::make(&a), SAnyRef::make(&b) });

				CHECK(result == a+b);
			}
		}

		//TODO
		
		SUBCASE("Pointers and references")
		{
			SUBCASE("void(struct*)")
			{
				CallableMember fn = CallableMember::make(&MyCallable::passStructPtr, { ParameterInfo(TypeName::create<MyCallable*>(), "v") });

				MyCallable objA;
				MyCallable objB;
				MyCallable* objB_ptr = &objB;
				fn.invoke(SAnyRef(), SAnyRef::make(&objA), { SAnyRef::make(&objB_ptr) });

				CHECK(objA.ptrCanary == &objB);
			}

			SUBCASE("void(struct&)")
			{
				CallableMember fn = CallableMember::make(&MyCallable::passStructRef, { ParameterInfo(TypeName::create<MyCallable&>(), "v") });

				MyCallable objA;
				MyCallable objB;
				fn.invoke(SAnyRef(), SAnyRef::make(&objA), { SAnyRef::make(&objB) });

				CHECK(objA.ptrCanary == &objB);
			}

			SUBCASE("struct*(struct*)")
			{
				CallableMember fn = CallableMember::make(&MyCallable::passthroughStructPtr, { ParameterInfo(TypeName::create<MyCallable*>(), "v") });

				MyCallable objA;
				MyCallable objB;
				MyCallable* objB_ptr = &objB;
				MyCallable* returnVal;
				fn.invoke(SAnyRef::make(&returnVal), SAnyRef::make(&objA), { SAnyRef::make(&objB_ptr) });

				CHECK(objA.ptrCanary == &objB);
			}

			SUBCASE("struct&(struct&)")
			{
				CallableMember fn = CallableMember::make(&MyCallable::passthroughStructRef, { ParameterInfo(TypeName::create<MyCallable&>(), "v") });

				MyCallable objA;
				MyCallable objB;
				MyCallable returnVal;
				fn.invoke(SAnyRef::make(&returnVal), SAnyRef::make(&objA), { SAnyRef::make(&objB) });

				CHECK(objA.ptrCanary == &objB);
			}

			SUBCASE("struct*()")
			{
				MyCallable objA;
				MyCallable objB;
				objA.ptrCanary = &objB;

				CallableMember fn = CallableMember::make(&MyCallable::returnStructPtr, {});

				MyCallable* returnVal;
				fn.invoke(SAnyRef::make(&returnVal), SAnyRef::make(&objA), {});

				CHECK(returnVal == objA.ptrCanary);
			}

			SUBCASE("struct&()")
			{
				MyCallable objA;
				MyCallable objB;
				objA.ptrCanary = &objB;
				objB.canary = 456;

				CallableMember fn = CallableMember::make(&MyCallable::returnStructRef, {});

				MyCallable returnVal;
				fn.invoke(SAnyRef::make(&returnVal), SAnyRef::make(&objA), {});

				CHECK(returnVal.canary == objB.canary);
			}
		}

		/*
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
