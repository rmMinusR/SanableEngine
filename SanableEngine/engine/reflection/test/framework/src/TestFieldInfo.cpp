#include <doctest/doctest.h>

#include "GlobalTypeRegistry.hpp"
#include "MemberInfo.hpp"
#include "PluginCore.hpp"

#include "SimpleStruct.hpp"

TEST_CASE("FieldInfo")
{
	//Prepare clean state
	{
		GlobalTypeRegistry::clear();
		ModuleTypeRegistry m;
		plugin_reportTypes(&m);
		GlobalTypeRegistry::loadModule("test runner", m);
	}

	const TypeInfo* ti = TypeName::create<SimpleStruct>().resolve();
	REQUIRE(ti != nullptr);

	SUBCASE("getValue (read)")
	{
		SimpleStruct testClass;
		const FieldInfo* fi;
		#define FIELD_VALS_EQ(fieldName) ((fi=ti->getField(#fieldName)) && *((decltype(testClass.fieldName)*)fi->getValue(&testClass)) == testClass.fieldName)
		CHECK(FIELD_VALS_EQ(a));
		CHECK(FIELD_VALS_EQ(b));
		CHECK(FIELD_VALS_EQ(c));
		CHECK(FIELD_VALS_EQ(d));
		#undef FIELD_VALS_EQ
	}
}
