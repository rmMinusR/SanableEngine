#include <doctest.h>

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

	static int privateFn()
	{
		return 123;
	}

	int privateMemFn()
	{
		return 456;
	}

public:
	static ptrdiff_t builtin_offsetof_priv()
	{
		return offsetof(PublicCastTester, priv);
	}

	static auto get_privateFn() { return &privateFn; }
	static auto get_privateMemFn() { return &PublicCastTester::privateMemFn; }
};

//Test rig: grant access to members
PUBLIC_CAST_GIVE_ACCESS(PublicCastTester, PUBLIC_CAST_PTR_TO_FIELD    (PublicCastTester, int), priv);
PUBLIC_CAST_GIVE_ACCESS(PublicCastTester, PUBLIC_CAST_PTR_TO_STATIC_FN(PublicCastTester, int), privateFn);
PUBLIC_CAST_GIVE_ACCESS(PublicCastTester, PUBLIC_CAST_PTR_TO_BOUND_FN (PublicCastTester, int), privateMemFn);

TEST_CASE("public_cast")
{
	SUBCASE("Basic assignment")
	{
		PublicCastTester tester;
		tester.*DO_PUBLIC_CAST(PublicCastTester, priv) = 2;
	}

	SUBCASE("Offsetof member variable")
	{
		PublicCastTester* tester = nullptr;
		ptrdiff_t diff = (char*)std::addressof(tester->*DO_PUBLIC_CAST(PublicCastTester, priv)) - (char*)tester;
		CHECK(diff == PublicCastTester::builtin_offsetof_priv());
		
		//auto good = DO_PUBLIC_CAST(PublicCastTester, int, priv);
		//auto bad = DO_PUBLIC_CAST(PublicCastTester, float, priv);
		//auto crash = tester->*DO_PUBLIC_CAST(PublicCastTester, float, priv);
	}

	SUBCASE("Static function")
	{
		auto fnPtr = DO_PUBLIC_CAST(PublicCastTester, privateFn);
		REQUIRE(fnPtr == PublicCastTester::get_privateFn());
		int res = fnPtr();
		CHECK(res == 123);
	}

	SUBCASE("Member function")
	{
		PublicCastTester tester;
		auto fp = DO_PUBLIC_CAST(PublicCastTester, privateMemFn);
		REQUIRE(fp == PublicCastTester::get_privateMemFn());
		int res = (tester.*fp)();
		CHECK(res == 456);
	}
}
