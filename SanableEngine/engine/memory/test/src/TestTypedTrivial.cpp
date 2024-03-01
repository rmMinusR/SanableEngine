#include <doctest/doctest.h>

#include "TypedMemoryPool.hpp"

TEST_CASE("TypedMemoryPool")
{
	SUBCASE("Emplace ints")
	{
		//Setup
		constexpr int nInts = 4;
		GenericTypedMemoryPool* poolBackend = GenericTypedMemoryPool::create<int>(nInts);
		TypedMemoryPool<int> pool(poolBackend);
		
		//Act
		int* vals[nInts];
		for (int i = 0; i < nInts; ++i)
		{
			vals[i] = pool.emplace(i);
			REQUIRE(vals[i] != nullptr); //Did allocate
		}

		//Check
		for (int i = 0; i < nInts; ++i)
		{
			CHECK(*(vals[i]) == i); //Holds correct value
			CHECK( reinterpret_cast<uint64_t>(vals[i]) % alignof(int) == 0 ); //Is correctly aligned
		}

		//Cleanup
		for (int i = 0; i < nInts; ++i) pool.release(vals[i]);
		delete poolBackend;
	}

	SUBCASE("Overallocate int")
	{
		//Set up pool and fill it
		GenericTypedMemoryPool* poolBackend = GenericTypedMemoryPool::create<int>(1);
		TypedMemoryPool<int> pool(poolBackend);
		int* goodAlloc = pool.emplace();
		REQUIRE(goodAlloc != nullptr);

		//Attempt to overalloc
		int* overAlloc = pool.emplace();
		CHECK(overAlloc == nullptr);

		//Clean up
		pool.release(goodAlloc);
		delete poolBackend;
	}

	SUBCASE("Leak ints")
	{
		constexpr int nInts = 4;
		GenericTypedMemoryPool* poolBackend = GenericTypedMemoryPool::create<int>(nInts);
		TypedMemoryPool<int> pool(poolBackend);
		for (int i = 0; i < nInts; ++i) pool.emplace(i);
	}
}
