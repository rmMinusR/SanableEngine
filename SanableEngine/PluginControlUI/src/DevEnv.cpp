#include "DevEnv.hpp"

#include <cassert>
#include <filesystem>

#include "application/Plugin.hpp"

// FIXME move to System

#if WIN32

extern Plugin const* plugin;

bool DevEnv::detect()
{
	return std::filesystem::exists(plugin->getPluginDir() / "dev_env.json");
}

void DevEnv::build(const std::wstring& targetName)
{

}

#else

bool DevEnv::detect()
{
	return false;
}

void DevEnv::build(const std::wstring& targetName)
{
	assert(false && "Not supported");
}

#endif
