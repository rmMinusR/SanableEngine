#include <doctest/doctest.h>

#include "ModuleTypeRegistry.hpp"
#include "TypeBuilder.hpp"

#include "TypedMemoryPool.hpp"
#include "MemoryMapper.hpp"
#include "MoveTester.hpp"

TEST_SUITE("MemoryMapper")
{
	TEST_CASE("Minimal")
	{
		//Setup
		int x = 123;
		int y;

		//Act
		MemoryMapper remapper;
		remapper.move(&y, &x);

		//Check
		CHECK(remapper.transformAddress(&x) == &y);
	}

	TEST_CASE("Mapper is null")
	{
		//Setup
		int x = 123;
		int y;

		//Act
		MemoryMapper* remapper = nullptr;
		remapper->move(&y, &x);

		//No check needed, since we would have errored
	}

	TEST_CASE("RawMemoryPool resize (uint8_t)")
	{
		//Setup
		constexpr size_t nObjs = 4;
		RawMemoryPool pool(nObjs, sizeof(uint8_t), alignof(uint8_t));
		uint8_t* objs[nObjs];
		for (int i = 0; i < nObjs; ++i)
		{
			objs[i] = (uint8_t*)pool.allocate();
			REQUIRE(objs[i]);
			*(objs[i]) = i;
		}

		//Act
		MemoryMapper remapper;
		pool.resizeObjects(sizeof(uint16_t), alignof(uint16_t), &remapper);
		uint8_t* remappedObjs[nObjs];
		for (int i = 0; i < nObjs; ++i)
		{
			remappedObjs[i] = remapper.transformAddress(objs[i]);
		}

		//Check
		for (int i = 0; i < nObjs; ++i)
		{
			CHECK(*(remappedObjs[i]) == i); //Address was remapped correctly
		}
	}

	TEST_CASE("RawMemoryPool count change (uint8_t)")
	{
		size_t startCount;
		size_t endCount;
		SUBCASE("Increase count") { startCount = 4; endCount = 8; }
		SUBCASE("Decrease count") { startCount = 8; endCount = 4; }
		SUBCASE("Same count"    ) { startCount = 4; endCount = 4; }
		size_t nToCreate = std::min(startCount, endCount);

		//Setup
		RawMemoryPool pool(startCount, sizeof(uint8_t), alignof(uint8_t));
		uint8_t** objs = new uint8_t*[nToCreate];
		for (int i = 0; i < nToCreate; ++i)
		{
			objs[i] = (uint8_t*)pool.allocate();
			REQUIRE(objs[i]);
			*(objs[i]) = i;
		}

		//Act
		MemoryMapper remapper;
		pool.setMaxNumObjects(endCount, &remapper);
		uint8_t** remappedObjs = new uint8_t*[nToCreate];
		for (int i = 0; i < nToCreate; ++i)
		{
			remappedObjs[i] = remapper.transformAddress(objs[i]);
		}

		//Check
		for (int i = 0; i < nToCreate; ++i)
		{
			CHECK(*(remappedObjs[i]) == i); //Address was remapped correctly
		}

		//Cleanup
		delete[] objs;
		delete[] remappedObjs;
	}

	TEST_CASE("TypedMemoryPool object resize (MoveTester)")
	{
		//Setup: Dummy type with double size/align
		TypeInfo realType = TypeInfo::createDummy<MoveTester>();
		TypeInfo dummyType = realType;
		dummyType.name = TypeName::create<MoveTester>();
		dummyType.layout.size *= 2;
		dummyType.layout.align *= 2;

		//Setup: Subcases
		TypeInfo* startingType = nullptr;
		TypeInfo* switchType = nullptr;
		SUBCASE("Increase size") { startingType = &realType ; switchType = &dummyType; }
		SUBCASE("Decrease size") { startingType = &dummyType; switchType = &realType ; }
		SUBCASE("Same size"    ) { startingType = &realType ; switchType = &realType ; }
		REQUIRE(startingType);
		REQUIRE(switchType);

		//Setup: Memory pool
		constexpr size_t nObjs = 4;
		GenericTypedMemoryPool* poolBackend = GenericTypedMemoryPool::create<MoveTester>(nObjs);
		TypedMemoryPool<MoveTester> pool(poolBackend);
		poolBackend->refreshObjects(*startingType, nullptr);

		//Setup: Objects in memory pool
		MoveTester* objs[nObjs];
		for (int i = 0; i < nObjs; ++i)
		{
			objs[i] = (MoveTester*) pool.emplace();
			REQUIRE(objs[i]);
		}

		//Act: Do resize
		MemoryMapper remapper;
		poolBackend->refreshObjects(*switchType, &remapper);
		MoveTester* remappedObjs[nObjs];
		for (int i = 0; i < nObjs; ++i)
		{
			remappedObjs[i] = remapper.transformAddress(objs[i]);
		}

		//Check
		for (int i = 0; i < nObjs; ++i)
		{
			CHECK(remappedObjs[i]->a == 1);
			CHECK(remappedObjs[i]->b == 2);
			CHECK(remappedObjs[i]->c == 3);
		}

		//Cleanup
		for (int i = 0; i < nObjs; ++i) pool.release(remappedObjs[i]);
		delete poolBackend;
	}


}
