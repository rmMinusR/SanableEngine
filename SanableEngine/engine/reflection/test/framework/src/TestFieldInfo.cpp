#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

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
	CHECK(ti != nullptr);

	SUBCASE("getValue (read)")
	{
		TestMemberRW testClass;
		const FieldInfo* fi;
		#define FIELD_VALS_EQ(fieldName) ((fi=ti->getField(#fieldName)) && *((decltype(testClass.fieldName)*)fi->getValue(&testClass)) == testClass.fieldName)
		CHECK(FIELD_VALS_EQ(a));
		CHECK(FIELD_VALS_EQ(b));
		CHECK(FIELD_VALS_EQ(c));
		CHECK(FIELD_VALS_EQ(d));
	}
}
