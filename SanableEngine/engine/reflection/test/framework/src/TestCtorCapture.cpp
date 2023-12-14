#include "doctest.h"

#include "GlobalTypeRegistry.hpp"
#include "CtorCapture.hpp"
#include "application/PluginCore.hpp"

#include "Inheritance.hpp"
#include "MultiInheritance.hpp"
#include "VirtualInheritance.hpp"

template<typename T>
void testCtorCaptureV2();

TEST_CASE("Ctor capture: Simple case")
{
	//Prepare clean state
	{
		GlobalTypeRegistry::clear();
		ModuleTypeRegistry m;
		plugin_reportTypes(&m);
		GlobalTypeRegistry::loadModule("test runner", m);
	}

	//Simple case
	SUBCASE("Derived1") { testCtorCaptureV2<Derived1>(); }
	SUBCASE("Derived2") { testCtorCaptureV2<Derived2>(); }
	SUBCASE("GrandchildOfBase")
	{ 
		testCtorCaptureV2<GrandchildOfBase>();
	}
}

TEST_CASE("Ctor capture: Multiple case")
{
	//Complex case: Multiple inheritance
	SUBCASE("ConcreteBase") { testCtorCaptureV2<ConcreteBase>(); }
	SUBCASE("ImplementerA") { testCtorCaptureV2<ImplementerA>(); }
	SUBCASE("ImplementerB") { testCtorCaptureV2<ImplementerB>(); }
}

TEST_CASE("Ctor capture: Virtual case")
{
	//Complex-er case: Multiple virtual inheritance
	SUBCASE("VirtualSharedBase") { testCtorCaptureV2<VirtualSharedBase>(); }
	SUBCASE("VirtualInheritedA") { testCtorCaptureV2<VirtualInheritedA>(); }
	SUBCASE("VirtualInheritedB") { testCtorCaptureV2<VirtualInheritedB>(); }
	SUBCASE("VirtualDiamond"   ) { testCtorCaptureV2<VirtualDiamond   >(); }
}


template<typename T>
void testCtorCaptureV2()
{
	SUBCASE("Parity vs self")
	{
		DetectedConstants vtables1 = capture_utils::type<T>::template ctor<>::captureVtables();
		DetectedConstants vtables2 = capture_utils::type<T>::template ctor<>::captureVtables();

		//Verify consistent
		for (size_t i = 0; i < sizeof(T); ++i)
		{
			REQUIRE(vtables1.usage[i] == vtables2.usage[i]);
			REQUIRE(vtables1.bytes[i] == vtables2.bytes[i]);
		}

		printf("============== v2/v2 parity: OK ==============\n");
	}

	SUBCASE("Parity vs v1")
	{
		for (uint8_t fill : { 0x00, 0x88, 0xFF })
		{
			const TypeInfo* ty = TypeName::create<T>().resolve();

			//Baseline
			uint8_t dummy[sizeof(T)];
			memset(dummy, fill, sizeof(T));
			ty->vptrJam(dummy);

			for (size_t i = 0; i < sizeof(T); ++i) printf("%02x ", (uint32_t)dummy[i]);
			printf("\n");

			//New version
			bool wasPreZeroed = true; //TODO detect pre-zeroing
			DetectedConstants vtables = capture_utils::type<T>::template ctor<>::captureVtables();

			//Unmark explicit fields
			ty->walkFields(
				[&](const FieldInfo& fi)
				{
					for (int i = 0; i < fi.size; ++i) vtables.usage[fi.offset + i] = false;
				}
			);
			
			for (size_t i = 0; i < sizeof(T); ++i) if (vtables.usage[i]) printf("%02x ", (uint32_t)vtables.bytes[i]); else printf(".. ");
			printf("\n");
			
			//Verify consistent
			for (size_t i = 0; i < sizeof(T); ++i)
			{
				if (vtables.usage[i]) REQUIRE(dummy[i] == vtables.bytes[i]);
				else REQUIRE(dummy[i] == fill);
			}

			printf("============== v1/v2 parity: OK ==============\n");

			if (wasPreZeroed) return; //Cannot test against varying fill if pre-zeroing is happening
		}
	}
}