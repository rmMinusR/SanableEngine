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

	typedef int (*privateFn_ptr)();
	typedef int (PublicCastTester::* privateMemFn_ptr)();

	static privateFn_ptr get_privateFn() { return &privateFn; }
	static privateMemFn_ptr get_privateMemFn() { return &PublicCastTester::privateMemFn; }
};

//Test rig: grant access to members
PUBLIC_CAST_GIVE_ACCESS(PublicCastTester, priv);
PUBLIC_CAST_GIVE_ACCESS(PublicCastTester, privateFn);
PUBLIC_CAST_GIVE_ACCESS(PublicCastTester, privateMemFn);

TEST_CASE("public_cast")
{
	SUBCASE("Basic assignment")
	{
		PublicCastTester tester;
		tester.*DO_PUBLIC_CAST(PublicCastTester, int, priv) = 2;
	}

	SUBCASE("Offsetof member variable")
	{
		PublicCastTester* tester = nullptr;
		ptrdiff_t diff = (char*)std::addressof(tester->*DO_PUBLIC_CAST(PublicCastTester, int, priv)) - (char*)tester;
		CHECK(diff == PublicCastTester::builtin_offsetof_priv());
	}

	SUBCASE("Static function")
	{
		PublicCastTester::privateFn_ptr fnPtr = DO_PUBLIC_CAST_RAW(PublicCastTester, PublicCastTester::privateFn_ptr, privateFn);
		CHECK(fnPtr == PublicCastTester::get_privateFn());
	}

	SUBCASE("Member function")
	{
		PublicCastTester tester;
		auto fp = DO_PUBLIC_CAST_RAW(PublicCastTester, PublicCastTester::privateMemFn_ptr, privateMemFn);
		REQUIRE(fp == PublicCastTester::get_privateMemFn());
		int res = (tester.*fp)();
		CHECK(res == 456);
	}
}
