#include <doctest/doctest.h>

#include "ModuleTypeRegistry.hpp"
#include "TypeBuilder.hpp"

#include "TypedMemoryPool.hpp"
#include "MemoryMapper.hpp"
#include "MoveTester.hpp"
#include "PointerTypes.hpp"
#include "MemoryRoot.hpp"
#include "MemoryHeap.hpp"

TEST_SUITE("MemoryMapper")
{
	TEST_CASE("Minimal: bare int pointer")
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

	TEST_CASE("Pointer within object")
	{
		//Env setup
		GlobalTypeRegistry::clear();
		{
			ModuleTypeRegistry reg;
			test_reportTypes(&reg);
			GlobalTypeRegistry::loadModule("Remapping test helpers", reg);
		}

		//Setup
		int x = 123;
		int y;
		PtrToInt ptrContainer;
		ptrContainer.target = &x;

		//Act
		MemoryMapper remapper;
		remapper.move(&y, &x);
		std::set<void*> log;
		remapper.transformObjectAddresses(&ptrContainer, TypeName::create<PtrToInt>(), true, &log);

		//Check
		CHECK(ptrContainer.target == &y);
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
		uint8_t* objs[nToCreate];
		for (int i = 0; i < nToCreate; ++i)
		{
			objs[i] = (uint8_t*)pool.allocate();
			REQUIRE(objs[i]);
			*(objs[i]) = i;
		}

		//Act
		MemoryMapper remapper;
		pool.setMaxNumObjects(endCount, &remapper);
		uint8_t* remappedObjs[nToCreate];
		for (int i = 0; i < nToCreate; ++i)
		{
			remappedObjs[i] = remapper.transformAddress(objs[i]);
		}

		//Check
		for (int i = 0; i < nToCreate; ++i)
		{
			CHECK(*(remappedObjs[i]) == i); //Address was remapped correctly
		}
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

TEST_CASE("MemoryRoot remapping" * doctest::timeout(1))
{
	GlobalTypeRegistry::clear();
	{
		ModuleTypeRegistry reg;
		test_reportTypes(&reg);
		GlobalTypeRegistry::loadModule("Remapping test helpers", reg);
	}

	SUBCASE("Pointer-to-int within object")
	{
		SUBCASE("External")
		{
			//Setup
			int x = 123;
			int y;
			PtrToInt ptrContainer;
			ptrContainer.target = &x;
			MemoryRoot::get()->registerExternal(&ptrContainer);
		
			//Act
			MemoryMapper remapper;
			remapper.move(&y, &x);
			std::set<void*> log;
			MemoryRoot::get()->updatePointers(remapper, log);

			//Check
			CHECK(ptrContainer.target == &y);

			//Cleanup
			MemoryRoot::get()->removeExternal(&ptrContainer);
		}

		SUBCASE("Heap")
		{
			//Setup
			int x = 123;
			int y;
			MemoryHeap heap;
			PtrToInt* ptrContainer = heap.create<PtrToInt>();
			ptrContainer->target = &x;
			MemoryRoot::get()->registerExternal(ptrContainer);

			//Act
			MemoryMapper remapper;
			remapper.move(&y, &x);
			std::set<void*> log;
			MemoryRoot::get()->updatePointers(remapper, log);

			//Check
			CHECK(ptrContainer->target == &y);

			//Cleanup
			MemoryRoot::get()->removeExternal(ptrContainer);
		}
	}

	SUBCASE("Double pointer within object")
	{
		//Env setup
		GlobalTypeRegistry::clear();
		{
			ModuleTypeRegistry reg;
			test_reportTypes(&reg);
			GlobalTypeRegistry::loadModule("Remapping test helpers", reg);
		}

		SUBCASE("Backing value remap")
		{
			//Setup
			int x = 123;
			int y;
			int* px = &x;
			DoublePtrToInt ptrContainer;
			ptrContainer.target = &px;
			MemoryRoot::get()->registerExternal(&ptrContainer);

			//Act
			MemoryMapper remapper;
			remapper.move(&y, &x);
			std::set<void*> log;
			MemoryRoot::get()->updatePointers(remapper, log);

			//Check
			CHECK(ptrContainer.target == &px); //Expect unchanged
			
			//Cleanup
			MemoryRoot::get()->removeExternal(&ptrContainer);
		}

		SUBCASE("Pointer remap")
		{
			//Setup
			int x = 123;
			int* px = &x;
			int* py;
			DoublePtrToInt ptrContainer;
			ptrContainer.target = &px;
			MemoryRoot::get()->registerExternal(&ptrContainer);

			//Act
			MemoryMapper remapper;
			remapper.move(&py, &px);
			std::set<void*> log;
			MemoryRoot::get()->updatePointers(remapper, log);

			//Check
			CHECK(ptrContainer.target == &py);
			
			//Cleanup
			MemoryRoot::get()->removeExternal(&ptrContainer);
		}

		SUBCASE("Simultaneous A")
		{
			//Setup
			int x = 123;
			int y;
			int* px = &x;
			int* py;
			DoublePtrToInt ptrContainer;
			ptrContainer.target = &px;
			MemoryRoot::get()->registerExternal(&ptrContainer);

			//Act
			MemoryMapper remapper;
			remapper.move(&y, &x);
			remapper.move(&py, &px);
			std::set<void*> log;
			MemoryRoot::get()->updatePointers(remapper, log);

			//Check
			CHECK(ptrContainer.target == &py);
			CHECK(*ptrContainer.target == &y);

			//Cleanup
			MemoryRoot::get()->removeExternal(&ptrContainer);
		}

		SUBCASE("Simultaneous B") //Same as before, but swapping move order
		{
			//Setup
			int x = 123;
			int y;
			int* px = &x;
			int* py;
			DoublePtrToInt ptrContainer;
			ptrContainer.target = &px;
			MemoryRoot::get()->registerExternal(&ptrContainer);

			//Act
			MemoryMapper remapper;
			remapper.move(&py, &px);
			remapper.move(&y, &x);
			std::set<void*> log;
			MemoryRoot::get()->updatePointers(remapper, log);

			//Check
			CHECK(ptrContainer.target == &py);
			CHECK(*ptrContainer.target == &y);

			//Cleanup
			MemoryRoot::get()->removeExternal(&ptrContainer);
		}
	}

	SUBCASE("Linked chain")
	{
		//Setup
		constexpr size_t chainLen = 3;
		CylicalPtrsLinear chain[chainLen];
		for (size_t i = 0; i < chainLen-1; ++i) chain[i].target = &chain[i+1];
		SUBCASE("Looped chain") { chain[chainLen-1].target = &chain[0]; }

		constexpr size_t movedIndex = 1;
		static_assert(movedIndex != 0); //Will fail if this isn't the looped chain case
		static_assert(movedIndex < chainLen);
		for (size_t i = 0; i < chainLen; ++i)
		{
			if (i != movedIndex) MemoryRoot::get()->registerExternal(&chain[i]);
		}
		CylicalPtrsLinear displaced;
		MemoryRoot::get()->registerExternal(&displaced);

		//Act
		MemoryMapper mover;
		mover.move(&displaced, &chain[movedIndex]);
		std::set<void*> log;
		MemoryRoot::get()->updatePointers(mover, log);

		//Check
		CHECK(chain[movedIndex-1].target == &displaced);

		//Cleanup
		for (size_t i = 1; i < chainLen; ++i)
		{
			if (i != movedIndex) MemoryRoot::get()->removeExternal(&chain[i]);
		}
		MemoryRoot::get()->removeExternal(&displaced);
	}

	//TODO SUBCASE("Linked tree") {}
}
