#include "application/Plugin.hpp"

#include "application/Application.hpp"
#include "application/PluginCore.hpp"
#include "GlobalTypeRegistry.hpp"
#include "MemoryHeap.hpp"
#include "MemoryRoot.hpp"

#if __EMSCRIPTEN__
#include <dlfcn.h>
#endif

#include <cassert>

Plugin::Plugin(const std::filesystem::path& pluginDir, const std::wstring& dllSubpath, SerialFile const* manifest) :
	status(Status::NotLoaded),
	reportedData(nullptr),
	manifest(manifest),
	pluginDir(pluginDir),
	dllSubpath(dllSubpath)
{
	dll = InvalidLibHandle;
}

Plugin::~Plugin()
{
	if (reportedData) delete reportedData;
	assert(!isCodeLoaded() && status == Status::NotLoaded && !isHooked());
}

Plugin::Plugin(Plugin&& mov) noexcept
{
	pluginDir = mov.pluginDir;
	dllSubpath = mov.dllSubpath;
	dll    = mov.dll;
	status = mov.status;

	mov.pluginDir.clear();
	mov.dllSubpath.clear();
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

std::filesystem::path Plugin::getPluginDir() const
{
	return pluginDir;
}

std::filesystem::path Plugin::getDllPath() const
{
	return pluginDir/dllSubpath;
}

std::wstring Plugin::getName() const
{
	size_t sepIdx = dllSubpath.rfind(L'.');
	if (sepIdx == std::wstring::npos) return dllSubpath;
	else return dllSubpath.substr(0, sepIdx);
}

bool Plugin::isCodeLoaded() const
{
	return dll != InvalidLibHandle;
}

bool Plugin::isHooked() const
{
	return status >= Status::Hooked;
}

const ModuleTypeRegistry* Plugin::getRTTI() const
{
	return GlobalTypeRegistry::getModule(reportedData->name);
}

bool Plugin::load(Application const* context)
{
	if (status != Status::NotLoaded) return status > Status::NotLoaded;
	assert(!isCodeLoaded());

	//Load code
#ifdef _WIN32
	dll = LoadLibraryW(getDllPath().c_str());
#endif
#ifdef __EMSCRIPTEN__
	dll = dlopen(getDllPath().c_str(), RTLD_LAZY);
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
		wprintf(L"ERROR: Plugin %s is missing report points\n", dllSubpath.c_str());
		return false;
	}
	if ((entryPoints.init==nullptr) != (entryPoints.cleanup==nullptr))
	{
		wprintf(L"ERROR: Plugin %s has mismatched hook points\n", dllSubpath.c_str());
		return false;
	}
	
	//Report plugin data
	if (!reportedData) reportedData = new PluginReportedData();
	bool success = entryPoints.report(this, reportedData, context);
	if (!success) return false;

	//Report RTTI
	ModuleTypeRegistry r;
	entryPoints.reportTypes(&r);
	GlobalTypeRegistry::loadModule(reportedData->name, r);
	wprintf(L"Loaded RTTI for %u types from plugin %s\n", r.getTypes().size(), dllSubpath.c_str());
	
	//If reloading, set release hooks
	if (wasEverLoaded)
	{
		ModuleTypeRegistry const* types = GlobalTypeRegistry::getModule(reportedData->name);
		MemoryRoot::get()->visitHeaps([&](MemoryHeap* heap)
		{
			for (const TypeInfo& i : types->getTypes())
			{
				GenericTypedMemoryPool* pool = heap->getSpecificPool(i.name);
				if (pool) pool->releaseHook = i.capabilities.rawDtor;
			}
		});
	}

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

	status = Status::Registered;

	return true;
}

void tryFreeWarnUnloaded(void* ptr)
{
	wprintf(L"WARNING: Attempted to free object with unloaded type at address: %p\n", ptr);
}

void Plugin::unload(Application* context)
{
	assert(status == Status::DllLoaded || status == Status::Registered);
	assert(isCodeLoaded());

	ModuleTypeRegistry const* types = GlobalTypeRegistry::getModule(reportedData->name);
	MemoryRoot::get()->visitHeaps([&](MemoryHeap* heap)
	{
		for (const TypeInfo& i : types->getTypes())
		{
			GenericTypedMemoryPool* pool = heap->getSpecificPool(i.name);
			if (pool) pool->releaseHook = tryFreeWarnUnloaded;
		}
	});
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