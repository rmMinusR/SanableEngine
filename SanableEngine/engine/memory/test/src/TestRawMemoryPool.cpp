#include "doctest.h"

#define TEST_MEMORY
#include "RawMemoryPool.hpp"



TEST_CASE("RawMemoryPool")
{
	SUBCASE("Allocate")
	{
		//Setup
		RawMemoryPool pool(1, sizeof(char));
		
		//Act
		void* obj = pool.allocate();
		CHECK(obj);
	}

	SUBCASE("Overallocate")
	{
		//Setup
		RawMemoryPool pool(1, sizeof(char));
		pool.allocate();

		CHECK(pool.allocate() == nullptr);
	}

	SUBCASE("Allocation tracking (num allocated, free, isAlive)")
	{
		//Setup
		constexpr size_t nObjs = 4;
		RawMemoryPool pool(nObjs, sizeof(char));
		CHECK(pool.getNumAllocatedObjects() == 0);
		CHECK(pool.getNumFreeObjects() == nObjs);

		//Act 1: allocate
		void* objs[nObjs];
		for (int i = 0; i < nObjs; ++i) objs[i] = pool.allocate();

		//Check 1
		CHECK(pool.getNumAllocatedObjects() == nObjs);
		CHECK(pool.getNumFreeObjects() == 0);
		for (int i = 0; i < nObjs; ++i) CHECK(pool.isAlive(objs[i]));

		//Act 2: deallocate an arbitrary object
		pool.release(objs[0]);
		CHECK(!pool.isAlive(objs[0]));

		//Check 2
		CHECK(pool.getNumAllocatedObjects() == nObjs-1);
		CHECK(pool.getNumFreeObjects() == 1);
	}
}



void* hookedObj = nullptr;
void hookTester(void* obj) { hookedObj = obj; }

TEST_CASE("RawMemoryPool")
{
	SUBCASE("Init hook")
	{
		//Clear state
		hookedObj = nullptr;

		//Setup
		RawMemoryPool pool(1, sizeof(char));
		pool.initHook = &hookTester;

		//Act
		void* obj = pool.allocate();
		REQUIRE(obj);
		CHECK(hookedObj == obj);

		pool.release(obj);
	}

	SUBCASE("Release hook (explicit)")
	{
		//Clear state
		hookedObj = nullptr;

		//Setup
		RawMemoryPool pool(1, sizeof(char));
		pool.releaseHook = &hookTester;

		//Act
		void* obj = pool.allocate();
		REQUIRE(obj);
		pool.release(obj);

		CHECK(hookedObj == obj);
	}

	SUBCASE("Release hook (leak)")
	{
		//Clear state
		hookedObj = nullptr;
		void* obj = nullptr;

		//Setup
		RawMemoryPool* pool = new RawMemoryPool(1, sizeof(char));
		pool->releaseHook = &hookTester;

		//More setup
		obj = pool->allocate();
		REQUIRE(obj);

		//Act: leak
		delete pool;

		CHECK(hookedObj == obj);
	}
}
