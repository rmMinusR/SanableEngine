#include "application/Plugin.hpp"

#include "application/PluginCore.hpp"
#include "GlobalTypeRegistry.hpp"

#if __EMSCRIPTEN__
#include <dlfcn.h>
#endif

#include <cassert>

Plugin::Plugin(const std::filesystem::path& path) :
	status(Status::NotLoaded),
	reportedData(nullptr),
	path(path)
{
	dll = InvalidLibHandle;
}

Plugin::~Plugin()
{
	assert(!isCodeLoaded() && status == Status::NotLoaded && !isHooked());
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
	assert(isCodeLoaded());
#ifdef _WIN32
	return reinterpret_cast<void*>(GetProcAddress(dll, name));
#endif
#ifdef __EMSCRIPTEN__
	return dlsym(dll, name);
#endif
}

std::filesystem::path Plugin::getPath() const
{
	return path;
}

bool Plugin::isCodeLoaded() const
{
	return dll != InvalidLibHandle;
}

bool Plugin::isHooked() const
{
	return status >= Status::Hooked;
}

bool Plugin::load(Application const* engine)
{
	if (status != Status::NotLoaded) return status > Status::NotLoaded;
	assert(!isCodeLoaded());

	//Load code
#ifdef _WIN32
	dll = LoadLibraryW(path.c_str());
#endif
#ifdef __EMSCRIPTEN__
	dll = dlopen(path.c_str(), RTLD_LAZY);
#endif

	//If load failed, abort
	if (!isCodeLoaded())
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

	//Gather entry points
	entryPoints.report      = (fp_plugin_report     ) getSymbol("plugin_report"     );
	entryPoints.init        = (fp_plugin_init       ) getSymbol("plugin_init"       );
	entryPoints.cleanup     = (fp_plugin_cleanup    ) getSymbol("plugin_cleanup"    );
	entryPoints.reportTypes = (fp_plugin_reportTypes) getSymbol("plugin_reportTypes");

	//Validate
	if (!entryPoints.report || !entryPoints.reportTypes)
	{
		wprintf(L"ERROR: Plugin %s is missing report points\n", path.filename().c_str());
		return false;
	}
	if ((entryPoints.init==nullptr) != (entryPoints.cleanup==nullptr))
	{
		wprintf(L"ERROR: Plugin %s has mismatched hook points\n", path.filename().c_str());
		return false;
	}
	
	//Report plugin data
	assert(!reportedData);
	reportedData = new PluginReportedData();
	bool success = entryPoints.report(this, reportedData, engine);
	if (!success) return false;

	//Report RTTI
	ModuleTypeRegistry r;
	entryPoints.reportTypes(&r);
	GlobalTypeRegistry::loadModule(reportedData->name, r);
	wprintf(L"Loaded RTTI for %u types from plugin %s\n", r.getTypes().size(), path.filename().c_str());
	
	status = Status::Registered;
	wasEverLoaded = true;
	return true;
}

bool Plugin::init()
{
	if (status != Status::Registered) return status > Status::Registered;
	assert(isCodeLoaded());

	if (entryPoints.init)
	{
		bool success = entryPoints.init(!wasEverHooked);
		if (!success) return false;
	}

	status = Status::Hooked;
	wasEverHooked = true;
	return true;
}

bool Plugin::cleanup(bool shutdown)
{
	if (status != Status::Hooked) return status < Status::Hooked;
	assert(isCodeLoaded());

	if (entryPoints.cleanup) entryPoints.cleanup(shutdown);

	assert(reportedData);
	delete reportedData;
	reportedData = nullptr;

	status = Status::Registered;

	return true;
}

void Plugin::unload()
{
	assert(status == Status::DllLoaded || status == Status::Registered);
	assert(isCodeLoaded());

	GlobalTypeRegistry::unloadModule(reportedData->name);

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