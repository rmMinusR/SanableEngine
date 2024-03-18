#include <doctest/doctest.h>

#include <cstddef>
#include <memory>

#include <public_cast.hpp>

//Test rig class
class PublicCastTester
{
public:
	int pub;
private:
	int priv;

	static int privateStaticFn(int arg1, int arg2)
	{
		return 123;
	}

	long privateMemFn(char arg1, void* arg2)
	{
		return 456;
	}

public:
	static ptrdiff_t builtin_offsetof_priv()
	{
		return offsetof(PublicCastTester, priv);
	}

	static auto get_privateFn() { return &privateStaticFn; }
	static auto get_privateMemFn() { return &PublicCastTester::privateMemFn; }
};

//Test rig: grant access to members
PUBLIC_CAST_GIVE_FIELD_ACCESS    (PublicCastTester_priv           , PublicCastTester, priv           , int);
PUBLIC_CAST_GIVE_STATIC_FN_ACCESS(PublicCastTester_privateStaticFn, PublicCastTester, privateStaticFn, int, int, int);
PUBLIC_CAST_GIVE_BOUND_FN_ACCESS (PublicCastTester_privateMemFn   , PublicCastTester, privateMemFn   , long, char, void*);

TEST_CASE("public_cast")
{
	SUBCASE("Basic assignment")
	{
		PublicCastTester tester;
		tester.*DO_PUBLIC_CAST(PublicCastTester_priv) = 2;
	}

	SUBCASE("Offsetof member variable (manual, explicit)")
	{
		PublicCastTester* tester = reinterpret_cast<PublicCastTester*>(0xDEADBEEF);
		ptrdiff_t diff = (char*)std::addressof(tester->*DO_PUBLIC_CAST(PublicCastTester_priv)) - (char*)tester;
		CHECK(diff == PublicCastTester::builtin_offsetof_priv());
	}

	SUBCASE("Offsetof member variable (macroed)")
	{
		ptrdiff_t diff = DO_PUBLIC_CAST_OFFSETOF_LAMBDA(PublicCastTester_priv, PublicCastTester)(nullptr);
		CHECK(diff == PublicCastTester::builtin_offsetof_priv());
	}

	/*
	//Disabled: Experimental and unproven. Must be regarded as unreliable.
	SUBCASE("Offsetof member variable (by reinterpret)")
	{
		auto boundPtr = DO_PUBLIC_CAST(PublicCastTester_priv);

		uint16_t raw;
		memcpy(&raw, &boundPtr, sizeof(raw));

		CHECK(raw == PublicCastTester::builtin_offsetof_priv());
	}
	*/

	SUBCASE("Static function")
	{
		auto fnPtr = DO_PUBLIC_CAST(PublicCastTester_privateStaticFn);
		REQUIRE(fnPtr == PublicCastTester::get_privateFn());
		int res = fnPtr(3, 6);
		CHECK(res == 123);
	}

	SUBCASE("Member function")
	{
		PublicCastTester tester;
		auto fp = DO_PUBLIC_CAST(PublicCastTester_privateMemFn);
		REQUIRE(fp == PublicCastTester::get_privateMemFn());
		int res = (tester.*fp)(1, nullptr);
		CHECK(res == 456);
	}
}
