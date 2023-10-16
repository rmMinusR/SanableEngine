#include "application/Plugin.hpp"

#include "application/PluginCore.hpp"
#include "GlobalTypeRegistry.hpp"

#if __EMSCRIPTEN__
#include <dlfcn.h>
#endif

#include <cassert>

__thiscall Plugin::Plugin(const std::filesystem::path& path) :
	status(Status::NotLoaded),
	reportedData(nullptr),
	path(path)
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
	return reinterpret_cast<void*>(GetProcAddress(dll, name));
#endif
#ifdef __EMSCRIPTEN__
	return dlsym(dll, name);
#endif
}

bool Plugin::_dllGood() const
{
	return dll != InvalidLibHandle;
}

bool Plugin::loadDLL()
{
	if (status != Status::NotLoaded) return status > Status::NotLoaded;
	assert(!_dllGood());

#ifdef _WIN32
	dll = LoadLibraryW(path.c_str());
#endif
#ifdef __EMSCRIPTEN__
	dll = dlopen(path.c_str(), RTLD_LAZY);
#endif
	if (!_dllGood())
	{
#ifdef _WIN32
		DWORD err = GetLastError();
		printf_s("Error: Code %u\n", err);
#endif
#ifdef __EMSCRIPTEN__
		printf("Error: %s\n", dlerror());
#endif
		return false;
	}

	status = Status::DllLoaded;
	return true;
}

bool Plugin::preInit(Application* engine)
{
	if (status != Status::DllLoaded) return status > Status::DllLoaded;
	assert(_dllGood());
	
	fp_plugin_preInit func = (fp_plugin_preInit)getSymbol("plugin_preInit");
	if (!func)
	{
		wprintf(L"ERROR: Plugin %s has no preInit function\n", path.filename().c_str());
		return false;
	}

	assert(!reportedData);
	reportedData = new PluginReportedData();
	bool success = func(this, reportedData, engine);
	if (!success) return false;

	status = Status::Registered;
	return true;
}

void Plugin::tryRegisterTypes()
{
	assert(_dllGood() && status >= Status::Registered);
	
	fp_plugin_reportTypes report = (fp_plugin_reportTypes)getSymbol("plugin_reportTypes");
	if (report)
	{
		ModuleTypeRegistry types;
		report(&types);
		GlobalTypeRegistry::loadModule(reportedData->name, types);

		wprintf(L"Loaded RTTI for %u types from plugin %s\n", types.getTypes().size(), path.filename().c_str());
	}
}

bool Plugin::init(bool firstRun)
{
	if (status != Status::Registered) return status > Status::Registered;
	assert(_dllGood());

	fp_plugin_init func = (fp_plugin_init)getSymbol("plugin_init");
	if (!func)
	{
		printf("ERROR: Plugin %s has no init function", (char*)path.filename().c_str());
		return false;
	}

	bool success = func(firstRun);
	if (!success) return false;

	status = Status::Hooked;

	return true;
}

bool Plugin::cleanup(bool shutdown)
{
	if (status != Status::Hooked) return status < Status::Hooked;
	assert(_dllGood());

	fp_plugin_cleanup func = (fp_plugin_cleanup)getSymbol("plugin_cleanup");
	if (!func) {
		printf("ERROR: Plugin %s has no cleanup function", (char*)path.filename().c_str());
		return false;
	}
	func(shutdown);

	assert(reportedData);
	delete reportedData;
	reportedData = nullptr;

	status = Status::Registered;

	return true;
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