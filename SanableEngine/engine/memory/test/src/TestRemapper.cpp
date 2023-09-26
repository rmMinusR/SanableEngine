#include "doctest.h"

#include "ModuleTypeRegistry.hpp"
#include "TypeBuilder.hpp"

#include "TypedMemoryPool.inl"
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

	TEST_CASE("GenericTypedMemoryPool resize (MoveTester)")
	{
		//Setup: Memory
		constexpr size_t nObjs = 4;
		TypedMemoryPool<MoveTester> pool(nObjs);
		MoveTester* objs[nObjs];
		for (int i = 0; i < nObjs; ++i)
		{
			objs[i] = (MoveTester*) pool.emplace();
			REQUIRE(objs[i]);
		}

		//Setup: Dummy type
		ModuleTypeRegistry reg;
		TypeBuilder::create<MoveTester>().registerType(&reg);
		TypeInfo dummyType = *reg.lookupType(TypeName::create<MoveTester>());
		dummyType.size *= 2; 
		dummyType.align *= 2; 

		//Act
		MemoryMapper remapper;
		pool.asGeneric()->refreshObjects(dummyType, &remapper);
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
	}
}
