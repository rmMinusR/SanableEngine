#include "DetectedConstants.hpp"

#include <cassert>

DetectedConstants::DetectedConstants(size_t sz)
{
	resize(sz);
}

void DetectedConstants::resize(size_t sz)
{
	//Wipe previous data
	bytes.clear();
	usage.clear();

	//Set up for new data
	bytes.resize(sz);
	usage.resize(sz);
}

void DetectedConstants::merge(const DetectedConstants& other)
{
	assert(other.bytes.size() == this->bytes.size());

	for (int i = 0; i < usage.size(); ++i)
	{
		this->usage[i] = this->usage[i]
				&& other.usage[i] //Ensure other has also identified it as const
				&& (other.bytes[i] == this->bytes[i]); //Ensure that our values match
	}
}

DetectedConstants::DetectedConstants()
{
}

DetectedConstants::~DetectedConstants()
{
}

DetectedConstants::DetectedConstants(const DetectedConstants& cpy)
{
	*this = cpy;
}

DetectedConstants::DetectedConstants(DetectedConstants&& mov)
{
	*this = std::move(mov);
}

DetectedConstants& DetectedConstants::operator=(const DetectedConstants& cpy)
{
	this->bytes = cpy.bytes;
	this->usage = cpy.usage;
	return *this;
}

DetectedConstants& DetectedConstants::operator=(DetectedConstants&& mov)
{
	this->bytes = std::move(mov.bytes);
	this->usage = std::move(mov.usage);
	mov.bytes.clear();
	mov.usage.clear();
	return *this;
}
