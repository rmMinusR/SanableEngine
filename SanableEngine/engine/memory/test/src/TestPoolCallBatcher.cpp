#include <doctest/doctest.h>

#include "ModuleTypeRegistry.hpp"
#include "GlobalTypeRegistry.hpp"
#include "TypeBuilder.hpp"
#include "ThunkUtils.hpp"

#include "MemoryHeap.hpp"
#include "PoolCallBatcher.hpp"

class MyCallable
{
public:
	virtual ~MyCallable() {}

	int canary = 0;

	void memFn() { ++canary; }
	virtual void virtFn() { ++canary; }
};

class ChildCallable : public MyCallable
{
public:
	virtual ~ChildCallable() {}

	int canary2 = 0;

	virtual void virtFn() override { ++canary2; }
};

void buildDummyRTTI(ModuleTypeRegistry* m)
{
	{
		TypeBuilder b = TypeBuilder::create<MyCallable>();
		b.addConstructor(stix::StaticFunction::make(&thunk_utils<MyCallable>::thunk_newInPlace<>), MemberVisibility::Public);
		b.captureClassImage_v2<MyCallable>();
		b.registerType(m);
	}

	{
		TypeBuilder b = TypeBuilder::create<ChildCallable>();
		b.addConstructor(stix::StaticFunction::make(&thunk_utils<ChildCallable>::thunk_newInPlace<>), MemberVisibility::Public);
		b.addParent<ChildCallable, MyCallable>(MemberVisibility::Public, ParentInfo::Virtualness::NonVirtual);
		b.captureClassImage_v2<ChildCallable>();
		b.registerType(m);
	}
}

TEST_CASE("PoolCallBatcher")
{
	//Prepare clean RTTI state
	{
		GlobalTypeRegistry::clear();
		ModuleTypeRegistry m;
		buildDummyRTTI(&m);
		GlobalTypeRegistry::loadModule("PoolCallBatcher dummies", m);
	}

	MemoryHeap memory;
	MyCallable* c1 = memory.create<MyCallable>();
	MyCallable* c2 = memory.create<MyCallable>();
	ChildCallable* c3 = memory.create<ChildCallable>();
	ChildCallable* c4 = memory.create<ChildCallable>();
	memory.ensureFresh();

	PoolCallBatcher<MyCallable> batcher;
	batcher.ensureFresh(&memory);

	SUBCASE("Member call")
	{
		REQUIRE(c1->canary == 0);
		REQUIRE(c2->canary == 0);
		batcher.memberCall(&MyCallable::memFn);
		CHECK(c1->canary == 1);
		CHECK(c2->canary == 1);
	}

	SUBCASE("Virtual call (indirect)")
	{
		REQUIRE(c1->canary == 0);
		REQUIRE(c2->canary == 0);
		REQUIRE(c3->canary == 0);
		REQUIRE(c4->canary == 0);
		REQUIRE(c3->canary2 == 0);
		REQUIRE(c4->canary2 == 0);
		batcher.memberCall(&MyCallable::virtFn);
		CHECK(c1->canary == 1);
		CHECK(c2->canary == 1);
		CHECK(c3->canary == 0);
		CHECK(c4->canary == 0);
		CHECK(c3->canary2 == 1);
		CHECK(c4->canary2 == 1);
	}

	SUBCASE("Added object of known type after PoolCallBatcher::ensureFresh")
	{
		MyCallable* c5 = memory.create<MyCallable>();
		REQUIRE(c5);
		REQUIRE(c5->canary == 0);
		batcher.memberCall(&MyCallable::memFn);
		CHECK(c5->canary == 1);
	}
}
