#include "doctest.h"

#include "TypedMemoryPool.inl"

TEST_CASE("TypedMemoryPool")
{
	SUBCASE("Emplace ints")
	{
		constexpr int nInts = 4;
		TypedMemoryPool<int> pool(nInts);
		
		int* vals[nInts];
		for (int i = 0; i < nInts; ++i)
		{
			vals[i] = pool.emplace(i);
			REQUIRE(vals[i] != nullptr); //Did allocate
			CHECK(*(vals[i]) == i); //Holds correct value
			CHECK( reinterpret_cast<uint64_t>(vals[i]) % alignof(int) == 0 ); //Is correctly aligned
		}

		for (int i = 0; i < nInts; ++i) pool.release(vals[i]);
	}

	SUBCASE("Overallocate int")
	{
		//Set up pool and fill it
		TypedMemoryPool<int> pool(1);
		int* goodAlloc = pool.emplace();
		REQUIRE(goodAlloc != nullptr);

		//Attempt to overalloc
		int* overAlloc = pool.emplace();
		CHECK(overAlloc == nullptr);

		//Clean up
		pool.release(goodAlloc);
	}

	SUBCASE("Leak ints")
	{
		constexpr int nInts = 4;
		TypedMemoryPool<int> pool(nInts);
		for (int i = 0; i < nInts; ++i) pool.emplace(i);
	}
}
