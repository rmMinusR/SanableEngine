#include "doctest.h"

#include "GlobalTypeRegistry.hpp"
#include "CtorCapture.hpp"
#include "application/PluginCore.hpp"

#include "Inheritance.hpp"


TEST_CASE("Ctor capture")
{
	//Prepare clean state
	{
		GlobalTypeRegistry::clear();
		ModuleTypeRegistry m;
		plugin_reportTypes(&m);
		GlobalTypeRegistry::loadModule("test runner", m);
	}

	SUBCASE("Derived1")
	{
		for (uint8_t fill : { 0x00, 0x88, 0xFF })
		{
			//Baseline
			uint8_t dummy[sizeof(Derived1)];
			memset(dummy, fill, sizeof(Derived1));
			TypeName::create<Derived1>().resolve()->vptrJam(dummy);

			//New version
			DetectedConstants vtables = capture_utils::type<Derived1>::ctor<>::captureVtables();

			for (size_t i = 0; i < sizeof(Derived1); ++i) printf("%x ", (uint32_t)dummy[i]);
			printf("\n");
			for (size_t i = 0; i < sizeof(Derived1); ++i) if (vtables.usage[i]) printf("%x ", (uint32_t)vtables.bytes[i]); else printf(".. ");
			printf("\n");

			//Verify consistent
			for (size_t i = 0; i < sizeof(Derived1); ++i)
			{
				if (vtables.usage[i]) CHECK(dummy[i] == vtables.bytes[i]);
				else CHECK(dummy[i] == fill);
			}
		}
	}
}
