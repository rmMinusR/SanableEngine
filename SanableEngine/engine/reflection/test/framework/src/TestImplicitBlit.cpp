#include "doctest.h"

#include "GlobalTypeRegistry.hpp"
#include "MemberInfo.hpp"
#include "PluginCore.hpp"

#include "SimpleStruct.hpp"
#include "Inheritance.hpp"

TEST_CASE("Implicit capture")
{
	/*
	//Prepare clean state
	{
		GlobalTypeRegistry::clear();
		ModuleTypeRegistry m;
		plugin_reportTypes(&m);
		GlobalTypeRegistry::loadModule("test runner", m);
	}

	//Setup
	const TypeInfo* td1 = m.lookupType(TypeName::create<Derived1>());
	const TypeInfo* td2 = m.lookupType(TypeName::create<Derived2>());
	REQUIRE(td1 != nullptr);
	REQUIRE(td2 != nullptr);

	Base* obj = new Derived1();
	CHECK(obj->identify() == Derived1::identify_s()); //Sanity

	td2->vptrJam(obj); //Transmute
	CHECK(obj->identify() == Derived2::identify_s());

	td1->vptrJam(obj); //Transmute back
	CHECK(obj->identify() == Derived1::identify_s());

	delete obj;
	// */
}
