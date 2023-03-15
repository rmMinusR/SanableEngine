#include "MemoryPoolCommon.hpp"

#include <cstdint>

#pragma region From original MemoryPool by Dean Lawson

//got this algorithm from: http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
int isPowerOfTwo(size_t x)
{
	return ((x != 0) && !(x & (x - 1)));
}

size_t getClosestPowerOf2LargerThan(size_t num)
{
	static uint32_t powersOf2[32];
	static bool arrayInitted = false;

	//init an array containing all the powers of 2 
	//(as it is static this should only run the first time this function is called)
	if (!arrayInitted)
	{
		for (uint32_t i = 0; i < 32; i++)
		{
			powersOf2[i] = 1 << i;
		}
	}

	//find the 1st power of 2 which is bigger than or equal to num
	for (uint32_t i = 0; i < 32; i++)
	{
		if ( powersOf2[i] >= num )
			return powersOf2[i];
	}

	//failsafe
	return 0;
	
}

#pragma endregion