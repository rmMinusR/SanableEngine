#include "doctest.h"

#include "FunctionInfo.hpp"

#include "Callables.hpp"

TEST_CASE("FunctionInfo")
{
	SUBCASE("Blank")
	{
		FunctionInfo fi;
		CHECK(!fi.isValid());
	}

	SUBCASE("Parity")
	{
		auto checkArgs_myFunc1 = [](const FunctionInfo& fi)
		{
			CHECK(fi.getReturnType() == TypeName::create<long>());
			CHECK(fi.getArgCount() == 0);
		};
		auto checkArgs_myFunc2 = [](const FunctionInfo& fi)
		{
			CHECK(fi.getReturnType() == TypeName::create<void>());
			CHECK(fi.getArgCount() == 0);
		};
		auto checkArgs_myFunc3 = [](const FunctionInfo& fi)
		{
			CHECK(fi.getReturnType() == TypeName::create<void>());
			REQUIRE(fi.getArgCount() == 1);
			CHECK(fi.getArg(0) == TypeName::create<int>());
		};
		auto checkArgs_myFunc4 = [](const FunctionInfo& fi)
		{
			CHECK(fi.getReturnType() == TypeName::create<void>());
			REQUIRE(fi.getArgCount() == 2);
			CHECK(fi.getArg(0) == TypeName::create<int>());
			CHECK(fi.getArg(1) == TypeName::create<char>());
		};

#define DECL_FN_TEST_ONE(group, accessDecl, enumDecl, isMember, accessPrefix) \
		SUBCASE(#accessPrefix "::" #group "_" #accessDecl) \
		{ \
			FunctionInfo fi = FunctionInfo::capture##enumDecl(&accessPrefix::group##_##accessDecl); \
			REQUIRE(fi.isValid()); \
			CHECK(fi.getCallConv() == FunctionInfo::CallConv::enumDecl); \
			checkArgs_##group(fi); \
			CHECK(fi.isMemberFunction() == isMember); \
		}

#define DECL_FN_TEST_GROUP(group) \
		DECL_FN_TEST_ONE(group, cdecl, CDecl, false, Callables) \
		DECL_FN_TEST_ONE(group, stdcall, StdCall, false, Callables) \
		DECL_FN_TEST_ONE(group, thiscall, ThisCall, true, Callables::CallableStruct)

		DECL_FN_TEST_GROUP(myFunc1)
		DECL_FN_TEST_GROUP(myFunc2)
		DECL_FN_TEST_GROUP(myFunc3)
		DECL_FN_TEST_GROUP(myFunc4)
	}
}
