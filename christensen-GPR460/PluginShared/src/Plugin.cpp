#include "Plugin.h"

#include <cassert>

#include "PluginCore.h"

bool Plugin::isLoaded()
{
	return GetModuleFileName(dll, NULL, 0) && status == Status::LoadComplete;
}

Plugin::Plugin(const std::filesystem::path& path) :
	path(path),
	dll((HMODULE)INVALID_HANDLE_VALUE),
	status(Status::NotLoaded)
{
}

Plugin::~Plugin()
{
	assert(!isLoaded());
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
	registerFunc(this);

	status = Status::Registered;
}

void Plugin::unloadDLL()
{
	assert(status >= Status::DllLoaded);
	assert(_dllGood());

	BOOL success = FreeLibrary(dll);
	assert(success);

	dll = HMODULE();
	status = Status::NotLoaded;
}