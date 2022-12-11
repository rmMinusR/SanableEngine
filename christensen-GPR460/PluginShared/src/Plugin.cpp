#include "Plugin.h"

#include <cassert>

#include "PluginCore.h"

__stdcall Plugin::Plugin(const std::filesystem::path& path) :
	path(path),
	dll((HMODULE)INVALID_HANDLE_VALUE),
	status(Status::NotLoaded)
{
}

Plugin::~Plugin()
{
	assert(!_dllGood() && status == Status::NotLoaded);
}

void Plugin::loadDLL()
{
	assert(status < Status::DllLoaded);
	assert(!_dllGood());

	dll = LoadLibrary(path.c_str());
	assert(dll != INVALID_HANDLE_VALUE);

	status = Status::DllLoaded;
}

void Plugin::registerContents()
{
	assert(status < Status::Registered);
	assert(_dllGood());
	
	fp_registerPlugin registerFunc = (fp_registerPlugin) GetProcAddress(dll, "registerPlugin");
	assert(registerFunc);
	registerFunc(this);

	status = Status::Registered;
}

void Plugin::unloadDLL()
{
	assert(status >= Status::DllLoaded);
	assert(_dllGood());

	BOOL success = FreeLibrary(dll);
	assert(success);

	dll = (HMODULE)INVALID_HANDLE_VALUE;
	status = Status::NotLoaded;
}