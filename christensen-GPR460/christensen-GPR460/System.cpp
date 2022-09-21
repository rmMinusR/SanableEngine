#include "System.hpp"

#include <cassert>

gpr460::System::System()
{
	isAlive = false;
}

gpr460::System::~System()
{
	assert(!isAlive);
}
