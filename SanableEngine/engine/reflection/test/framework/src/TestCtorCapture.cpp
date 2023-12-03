#include "doctest.h"

#include "GlobalTypeRegistry.hpp"
#include "CtorCapture.hpp"
#include "application/PluginCore.hpp"

#include "Inheritance.hpp"
#include "MultiInheritance.hpp"

template<typename T>
void testCtorCaptureV2();

TEST_CASE("Ctor capture")
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

	//Complex case
	SUBCASE("ConcreteBase") { testCtorCaptureV2<ConcreteBase>(); }
	SUBCASE("ImplementerA") { testCtorCaptureV2<ImplementerA>(); }
	SUBCASE("ImplementerB") { testCtorCaptureV2<ImplementerB>(); }
}


template<typename T>
void testCtorCaptureV2()
{
	for (uint8_t fill : { 0x00, 0x88, 0xFF })
	{
		//Baseline
		uint8_t dummy[sizeof(T)];
		memset(dummy, fill, sizeof(T));
		TypeName::create<T>().resolve()->vptrJam(dummy);

		//New version
		bool wasPreZeroed = false;
		DetectedConstants vtables = capture_utils::type<T>::template ctor<>::captureVtables(&wasPreZeroed);
		
		for (size_t i = 0; i < sizeof(T); ++i) printf("%2x ", (uint32_t)dummy[i]);
		printf("\n");
		for (size_t i = 0; i < sizeof(T); ++i) if (vtables.usage[i]) printf("%2x ", (uint32_t)vtables.bytes[i]); else printf(".. ");
		printf("\n");
		
		//Verify consistent
		for (size_t i = 0; i < sizeof(T); ++i)
		{
			if (vtables.usage[i]) REQUIRE(dummy[i] == vtables.bytes[i]);
			else REQUIRE(dummy[i] == fill);
		}

		if (wasPreZeroed) return; //Cannot test against varying fill if pre-zeroing is happening
	}
}