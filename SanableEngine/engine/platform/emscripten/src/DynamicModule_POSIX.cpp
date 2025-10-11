#include "DynamicModule_POSIX.hpp"

#include <dlfcn.h>

#include <cassert>
#include <iostream>

DynamicModule_POSIX::DynamicModule_POSIX(std::filesystem::path path) :
	DynamicModule(path)
{
	sharedObjectHandle = dlopen(path.wstring().c_str(), RTLD_LAZY);
	if (!sharedObjectHandle)
	{
		std::cerr << "Error: " << dlopen() << std::endl;
		assert(false);
	}
}

DynamicModule_POSIX::~DynamicModule_POSIX()
{
	int failure = dlclose(sharedObjectHandle);
	assert(!failure);
}

void* DynamicModule_POSIX::getSymbol(const char* name) const
{
	return reinterpret_cast<void*>(dlsym(sharedObjectHandle, name));
}
