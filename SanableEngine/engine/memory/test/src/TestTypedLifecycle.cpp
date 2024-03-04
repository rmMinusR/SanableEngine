#include <doctest/doctest.h>

#include "TypedMemoryPool.hpp"
#include "LeakTracer.hpp"

TEST_CASE("TypedMemoryPool")
{
	//Clean state
	LeakTracer::numLiving = 0;

	SUBCASE("Emplace LeakTracers")
	{
		constexpr int nObjs = 4;
		GenericTypedMemoryPool* poolBackend = GenericTypedMemoryPool::create<LeakTracer>(nObjs);
		TypedMemoryPool<LeakTracer> pool(poolBackend);
		
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

		delete poolBackend;
	}

	SUBCASE("Double-delete LeakTracers")
	{
		//Setup
		GenericTypedMemoryPool* poolBackend = GenericTypedMemoryPool::create<LeakTracer>(1);
		TypedMemoryPool<LeakTracer> pool(poolBackend);
		LeakTracer* obj = pool.emplace();
		REQUIRE(LeakTracer::numLiving == 1);

		//Act 1: first delete
		pool.release(obj);

		//Verify 1: make sure dtor was called
		CHECK(LeakTracer::numLiving == 0);

		//Act 2: delete again
		pool.release(obj);
		
		//Verify 2: make sure dtor wasn't called again
		CHECK(LeakTracer::numLiving == 0);

		delete poolBackend;
	}

	SUBCASE("Leak LeakTracers")
	{
		//Setup
		constexpr int nObjs = 4;
		GenericTypedMemoryPool* poolBackend = GenericTypedMemoryPool::create<LeakTracer>(nObjs);
		TypedMemoryPool<LeakTracer> pool(poolBackend);
		for (int i = 0; i < nObjs; ++i) pool.emplace();
		REQUIRE(LeakTracer::numLiving == nObjs);

		//Act
		delete poolBackend;

		//Verify
		CHECK(LeakTracer::numLiving == 0);
	}
}
