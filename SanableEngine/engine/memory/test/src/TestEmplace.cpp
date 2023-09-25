#include "doctest.h"

#define TEST_MEMORY 
#include "TypedMemoryPool.inl"

TEST_CASE("TypedMemoryPool::emplace")
{
	SUBCASE("Emplace int")
	{
		TypedMemoryPool<int> pool;
		int* val = pool.emplace(12345);
		REQUIRE(val != nullptr);
		CHECK(*val == 12345);
		CHECK( reinterpret_cast<uint64_t>(val) % alignof(int) == 0 ); //Is correctly aligned
		pool.release(val);
	}

	/*
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
	// */

	SUBCASE("Emplace many ints")
	{
		constexpr int nInts = 32;
		TypedMemoryPool<int> pool(nInts);
		int* vals[nInts];
		for (int i = 0; i < nInts; ++i)
		{
			vals[i] = pool.emplace(i);
			CHECK(vals[i] != nullptr); //Did allocate
			CHECK(*(vals[i]) == i); //Holds correct value
			//CHECK( reinterpret_cast<uint64_t>(vals[i]) % alignof(int) == 0 ); //Is correctly aligned
		}

		for (int i = 0; i < nInts; ++i) pool.release(vals[i]);
	}
}
