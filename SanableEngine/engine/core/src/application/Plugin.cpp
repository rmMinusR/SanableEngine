#include "application/Plugin.hpp"

#include "application/Application.hpp"
#include "application/PluginCore.hpp"
#include "System.hpp"
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
	handle = mov.handle;
	status = mov.status;

	mov.pluginDir.clear();
	mov.dllSubpath.clear();
	mov.handle = nullptr;
	mov.status = Status::NotLoaded;
}

void* Plugin::getSymbol(const char* name) const
{
	assert(isCodeLoaded());
	return handle->getSymbol(name);
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
	return handle != nullptr;
}

bool Plugin::isHooked() const
{
	return status >= Status::Hooked;
}

const ModuleTypeRegistry* Plugin::getRTTI() const
{
	return GlobalTypeRegistry::getModule(reportedData->name);
}

bool Plugin::load(Application* context)
{
	if (status != Status::NotLoaded) return status > Status::NotLoaded;
	assert(!isCodeLoaded());

	//Load code
	handle = context->getSystem()->loadDynamicModule(getDllPath());

	// TODO allow soft error again (handle init currently asserts and HCFs)

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

	delete handle;
	handle = nullptr;

	status = Status::NotLoaded;
}
