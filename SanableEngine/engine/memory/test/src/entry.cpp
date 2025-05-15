#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include "MemoryRoot.hpp"

int main(int argc, char** argv)
{
	auto rv = doctest::Context(argc, argv).run();
	MemoryRoot::cleanup();
	return rv;
}
