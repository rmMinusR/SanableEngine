#pragma once

class LeakTracer
{
public:
	static size_t numLiving;

	LeakTracer();
	~LeakTracer();
};
