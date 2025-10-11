#include "DynamicModule_Win32.hpp"

#include <cassert>
#include <iostream>

DynamicModule_Win32::DynamicModule_Win32(std::filesystem::path path) :
	DynamicModule(path)
{
	dllHandle = LoadLibraryW(path.wstring().c_str());
	if (dllHandle == INVALID_HANDLE_VALUE)
	{
		DWORD err = GetLastError();
		std::cerr << "Error: Code " << err << std::endl;
		assert(false);
	}
}

DynamicModule_Win32::~DynamicModule_Win32()
{
	FreeLibrary(dllHandle);
}

void* DynamicModule_Win32::getSymbol(const char* name) const
{
	return reinterpret_cast<void*>(GetProcAddress(dllHandle, name));
}
