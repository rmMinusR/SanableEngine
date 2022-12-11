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
	assert(status == Status::NotLoaded);
	assert(!_dllGood());

	dll = LoadLibrary(path.c_str());
	assert(dll != INVALID_HANDLE_VALUE);

	status = Status::DllLoaded;
}

void Plugin::plugin_preInit(EngineCore* engine)
{
	assert(status == Status::DllLoaded);
	assert(_dllGood());
	
	fp_plugin_preInit func = (fp_plugin_preInit)GetProcAddress(dll, "plugin_preInit");
	assert(func);
	bool success = func(this, engine);
	assert(success);

	status = Status::Registered;
}

void Plugin::init()
{
	assert(status == Status::Registered);
	assert(_dllGood());

	fp_plugin_init func = (fp_plugin_init)GetProcAddress(dll, "plugin_init");
	assert(func);
	bool success = func();
	assert(success);

	status = Status::Hooked;
}

void Plugin::plugin_cleanup()
{
	assert(status == Status::Hooked);
	assert(_dllGood());

	fp_plugin_cleanup func = (fp_plugin_cleanup)GetProcAddress(dll, "plugin_cleanup");
	assert(func);
	func();

	status = Status::Registered;
}

void Plugin::unloadDLL()
{
	assert(status == Status::DllLoaded || status == Status::Registered);
	assert(_dllGood());

	BOOL success = FreeLibrary(dll);
	assert(success);

	dll = (HMODULE)INVALID_HANDLE_VALUE;
	status = Status::NotLoaded;
}