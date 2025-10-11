#pragma once

#include <Windows.h>

#include "DynamicModule.hpp"


namespace gpr460 { class System_Win32; }


class DynamicModule_Win32 : public DynamicModule
{
public:
	virtual ~DynamicModule_Win32();

	virtual void* getSymbol(const char* name) const;

protected:
	friend class gpr460::System_Win32;
	DynamicModule_Win32(std::filesystem::path path);

	HMODULE dllHandle;
};
