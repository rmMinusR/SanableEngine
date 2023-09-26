#include "doctest.h"

#include "TypedMemoryPool.inl"
#include "LeakTracer.hpp"

TEST_CASE("TypedMemoryPool")
{
	//Clean state
	LeakTracer::numLiving = 0;

	SUBCASE("Emplace LeakTracers")
	{
		constexpr int nObjs = 4;
		TypedMemoryPool<LeakTracer> pool(nObjs);
		
		LeakTracer* objs[nObjs];
		for (int i = 0; i < nObjs; ++i)
		{
			objs[i] = pool.emplace();
			REQUIRE(objs[i] != nullptr); //Did allocate
			CHECK(reinterpret_cast<uint64_t>(objs[i]) % alignof(LeakTracer) == 0); //Is correctly aligned
		}

		CHECK(LeakTracer::numLiving == nObjs);

		for (int i = 0; i < nObjs; ++i) pool.release(objs[i]);

		CHECK(LeakTracer::numLiving == 0);
	}

	SUBCASE("Leak LeakTracers")
	{
		//Setup
		constexpr int nObjs = 4;
		TypedMemoryPool<LeakTracer>* pool = new TypedMemoryPool<LeakTracer>(nObjs);
		for (int i = 0; i < nObjs; ++i) pool->emplace();
		REQUIRE(LeakTracer::numLiving == nObjs);

		//Act
		delete pool;

		//Verify
		CHECK(LeakTracer::numLiving == 0);
	}
}
