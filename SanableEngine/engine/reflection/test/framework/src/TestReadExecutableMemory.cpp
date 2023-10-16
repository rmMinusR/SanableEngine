#include "doctest.h"

#include "GlobalTypeRegistry.hpp"
#include "application/PluginCore.hpp"

#include "MultiInheritance.hpp"

#include <iostream>

void testFunc()
{
	printf("Test!");
}

union Transmuter
{
	uint8_t* dataPtr;
	void (*fnPtr)();
};

void writeHex(uint8_t val)
{
	char lut[] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
	};

	std::cout << lut[val >> 4];
	std::cout << lut[val & 0xF];
}

TEST_CASE("Executable memory reading")
{
	Transmuter t;
	t.fnPtr = &testFunc;
	for (int i = 0; i < 64; ++i)
	{
		writeHex(t.dataPtr[i]);
		std::cout << " ";
	}
}
