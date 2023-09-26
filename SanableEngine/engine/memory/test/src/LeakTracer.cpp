#include "LeakTracer.hpp"

size_t LeakTracer::numLiving = 0;

LeakTracer::LeakTracer()
{
	numLiving++;
}

LeakTracer::~LeakTracer()
{
	numLiving--;
}
