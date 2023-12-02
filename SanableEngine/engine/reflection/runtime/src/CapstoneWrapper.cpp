#include "CapstoneWrapper.hpp"

#include <cassert>

csh capstone_instance;

void capstone_check_error(cs_err code)
{
	if (code != CS_ERR_OK)
	{
		printf("Error with Capstone: %s\n", cs_strerror(code));
		assert(false);
	}
}

csh capstone_get_instance()
{
	assert(cs_support(cs_arch::CS_ARCH_OURS));

	if (!capstone_instance)
	{
		cs_err status = cs_open(
			cs_arch::CS_ARCH_OURS,
			sizeof(void*) == 8 ? cs_mode::CS_MODE_64 : cs_mode::CS_MODE_32,
			&capstone_instance
		);
		capstone_check_error(status);
	}

	return capstone_instance;
}

void capstone_cleanup_instance()
{
	if (capstone_instance)
	{
		cs_err status = cs_close(&capstone_instance);
		capstone_check_error(status);
	}
}

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
