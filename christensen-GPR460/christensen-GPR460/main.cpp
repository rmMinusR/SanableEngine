#include <iostream>

#include "Logger.h"

int main()
{
	Logger::debug("Hello, world!");
	Logger::info("Hello, world!");
	Logger::warn("Hello, world!");
	Logger::error("Hello, world!");
	Logger::fatal("Hello, world!");

	return 0;
}
