#include "doctest.h"

#include "CtorCapture.hpp"

#include "Inheritance.hpp"

TEST_CASE("Ctor capture")
{
	auto vtables = capture_utils::type<Derived1>::ctor<>::captureVtables();
}
