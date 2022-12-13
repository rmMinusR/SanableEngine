#include "Plugin.hpp"

#if __EMSCRIPTEN__
#include <dlfcn.h>
#endif

#include <cassert>

#include "PluginCore.hpp"

__thiscall Plugin::Plugin(const std::filesystem::path& path) :
	path(path),
	status(Status::NotLoaded)
{
	dll = InvalidLibHandle;
}

Plugin::~Plugin()
{
	assert(!_dllGood() && status == Status::NotLoaded);
}

Plugin::Plugin(Plugin&& mov) noexcept
{
	path   = mov.path;
	dll    = mov.dll;
	status = mov.status;

	mov.path.clear();
	mov.dll = InvalidLibHandle;
	mov.status = Status::NotLoaded;
}

void* Plugin::getSymbol(const char* name) const
{
	assert(_dllGood());
#ifdef _WIN32
	return GetProcAddress(dll, name);
#endif
#ifdef __EMSCRIPTEN__
	return dlsym(dll, name);
#endif
}

bool Plugin::_dllGood() const
{
	return dll != InvalidLibHandle;
}

void Plugin::loadDLL()
{
	assert(status == Status::NotLoaded);
	assert(!_dllGood());

#ifdef _WIN32
	dll = LoadLibrary(path.c_str());
#endif
#ifdef __EMSCRIPTEN__
	dll = dlopen(path.c_str(), RTLD_LAZY);
#endif
	assert(_dllGood());
	
	status = Status::DllLoaded;
}

void Plugin::preInit(EngineCore* engine)
{
	assert(status == Status::DllLoaded);
	assert(_dllGood());
	
	fp_plugin_preInit func = (fp_plugin_preInit)getSymbol("plugin_preInit");
	assert(func);
	bool success = func(this, engine);
	assert(success);

	status = Status::Registered;
}

void Plugin::init()
{
	assert(status == Status::Registered);
	assert(_dllGood());

	fp_plugin_init func = (fp_plugin_init)getSymbol("plugin_init");
	assert(func);
	bool success = func();
	assert(success);

	status = Status::Hooked;
}

void Plugin::cleanup()
{
	assert(status == Status::Hooked);
	assert(_dllGood());

	fp_plugin_cleanup func = (fp_plugin_cleanup)getSymbol("plugin_cleanup");
	assert(func);
	func();

	status = Status::Registered;
}

void Plugin::unloadDLL()
{
	assert(status == Status::DllLoaded || status == Status::Registered);
	assert(_dllGood());

#ifdef _WIN32
	BOOL success = FreeLibrary(dll);
	assert(success);
#endif
#ifdef __EMSCRIPTEN__
	int failure = dlclose(dll);
	assert(!failure);
#endif

	dll = InvalidLibHandle;
	status = Status::NotLoaded;
}