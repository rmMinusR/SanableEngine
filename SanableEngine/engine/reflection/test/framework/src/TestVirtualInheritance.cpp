#include "doctest.h"

#include "GlobalTypeRegistry.hpp"
#include "MemberInfo.hpp"
#include "PluginCore.hpp"

#include "VirtualInheritance.hpp"

TEST_CASE("TypeInfo::upcast (virtual)")
{
	//Prepare clean state
	{
		GlobalTypeRegistry::clear();
		ModuleTypeRegistry m;
		plugin_reportTypes(&m);
		GlobalTypeRegistry::loadModule("test runner", m);
	}

	SUBCASE("Direct upcast to virtual")
	{
		const TypeInfo* ti = TypeName::create<VirtualInheritedA>().resolve();
		REQUIRE(ti != nullptr);

		VirtualInheritedA obj;
		REQUIRE(ti->upcast(&obj, TypeName::create<VirtualSharedBase>()) == (VirtualSharedBase*)&obj);
	}
}
