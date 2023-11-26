#include "doctest.h"

#include <ThunkCapture.hpp>

int myTargetFn1() { return 0; }
void myReferenceThunk() { myTargetFn1(); }

int myTargetFn2() { return 0; }
void myUnknownThunk() { myTargetFn2(); }

struct MyStruct
{
	MyStruct() {}
	void foo() {}
	void bar() {}
};

void MyStruct_ctor_thunk() { new(nullptr) MyStruct(); }
void MyStruct_foo_thunk() { ((MyStruct*)nullptr)->foo(); }
void MyStruct_bar_thunk() { ((MyStruct*)nullptr)->bar(); }

TEST_CASE("Thunk scanning")
{
	REQUIRE(&myTargetFn1 != &myTargetFn2);

	SUBCASE("CALL")
	{
		SUBCASE("Static")
		{
			auto marker = scanForNearCall(&myReferenceThunk, (_Fn<void>::ptr_t)myTargetFn1);
			SUBCASE("Parity")              CHECK((_Fn<void>::ptr_t)myTargetFn1 == getNearCallFnPtr(&myReferenceThunk, marker));
			SUBCASE("Indirect extraction") CHECK((_Fn<void>::ptr_t)myTargetFn2 == getNearCallFnPtr(&myUnknownThunk, marker));
		}

		/*
		SUBCASE("Member")
		{
			auto marker = scanForNearCall(&MyStruct_foo_thunk, (_Fn<void>::ptr_t)MyStruct::foo);
			SUBCASE("Parity")              CHECK((_Fn<void>::ptr_t)MyStruct::foo == getNearCallFnPtr(&MyStruct_foo_thunk, marker));
			SUBCASE("Indirect extraction") CHECK((_Fn<void>::ptr_t)MyStruct::bar == getNearCallFnPtr(&MyStruct_bar_thunk, marker));
		}
		*/
	}

}
