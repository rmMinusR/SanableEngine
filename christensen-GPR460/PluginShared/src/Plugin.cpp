#include "Plugin.h"

#include <cassert>

#include "PluginCore.h"

bool Plugin::isLoaded()
{
	return GetModuleFileName(dll, NULL, 0) && status == Status::LoadComplete;
}

Plugin::Plugin(const std::wstring& path) :
	path(path),
	dll(),
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
	assert(!GetModuleFileName(dll, NULL, 0));

	dll = LoadLibrary(path.c_str());

	status = Status::DllLoaded;
}

void Plugin::registerContents()
{
	assert(status < Status::Registered);
	assert(GetModuleFileName(dll, NULL, 0));
	
	fp_registerPlugin registerFunc = (fp_registerPlugin) GetProcAddress(dll, "registerPlugin");
	registerFunc(this);

	status = Status::Registered;
}

void Plugin::unloadDLL()
{
	assert(GetModuleFileName(dll, NULL, 0));
	assert(status >= Status::DllLoaded);

	BOOL success = FreeLibrary(dll);
	assert(success);

	dll = HMODULE();
	status = Status::NotLoaded;
}