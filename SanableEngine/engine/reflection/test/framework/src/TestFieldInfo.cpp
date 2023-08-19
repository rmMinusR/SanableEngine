#include "catch.hpp"

#include "GlobalTypeRegistry.hpp"
#include "MemberInfo.hpp"
#include "PluginCore.hpp"

#include "MemberRW.hpp"

TEST_CASE("FieldInfo")
{
	// Prepare clean state
	GlobalTypeRegistry::clear();
	ModuleTypeRegistry m;
	plugin_reportTypes(&m);

	const TypeInfo* ti = m.lookupType(TypeName::create<TestMemberRW>());
	REQUIRE(ti != nullptr);

	SECTION("getValue (read)")
	{
		TestMemberRW testClass;
		const FieldInfo* fi;
		#define FIELD_VALS_EQ(fieldName) (fi=ti->getField(#fieldName)) && *((decltype(testClass.fieldName)*)fi->getValue(&testClass)) == testClass.fieldName
		REQUIRE(FIELD_VALS_EQ(a));
		REQUIRE(FIELD_VALS_EQ(b));
		REQUIRE(FIELD_VALS_EQ(c));
		REQUIRE(FIELD_VALS_EQ(d));
	}
}
