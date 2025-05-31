#include "DevEnv.hpp"

#include <cassert>
#include <filesystem>
#include <sstream>

#include "application/Plugin.hpp"
#include "game/Game.hpp"
#include "System.hpp"
#include "data/SerialFile.hpp"
#include "data/SerialNode.hpp"

// FIXME move to System

#if WIN32

extern Game* game;
extern Plugin const* plugin;

bool DevEnv::detect()
{
	return std::filesystem::exists(plugin->getPluginDir() / "dev_env.json");
}

std::future<bool> DevEnv::buildAsync(const std::wstring& targetName)
{
	// Force a copy to persist while the process runs, otherwise we risk a dangling reference
	return std::async(+[](std::wstring targetName) { return build(targetName); }, targetName);
}

bool DevEnv::build(const std::wstring& targetName)
{
	SerialFile env(plugin->getPluginDir() / "dev_env.json");

	std::wstring cmakePath = static_cast<const SerialString*>( static_cast<const SerialObject*>(env.getRootNode())->get(L"cmakePath") )->value();
	std::wstring cmakeArgs = (std::wostringstream() << "--build . --target " << targetName).str();
	std::wstring workDir = game->getApplication()->getSystem()->GetBaseDir().wstring();

	STARTUPINFOW startup = { 0 };
	startup.cb = sizeof(startup);
	
	PROCESS_INFORMATION proc = { 0 };
	bool ok = CreateProcessW(
		cmakePath.c_str(),
		cmakeArgs.data(),
		NULL, NULL, // proc/thread security
		FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL, // env vars
		workDir.c_str(),
		&startup,
		&proc
	);
	if (!ok)
	{
		printf("ERROR: CreateProcessW code %i", (int)GetLastError());
		assert(false);
		return false;
	}

	int waitErr = WaitForSingleObject(proc.hProcess, INFINITE);
	if (waitErr != WAIT_OBJECT_0)
	{
		printf("ERROR: WaitForSingleObject code %i", (int)GetLastError());
		assert(false);

		// cmake may still be running, kill it just in case
		TerminateProcess(proc.hProcess, 0xDEAD2BAD);

		return false;
	}
	
	// cmake exited, get its return code
	DWORD exitCode = 0;
	if (!GetExitCodeProcess(proc.hProcess, &exitCode))
	{
		printf("ERROR: GetExitCodeProcess code %i", (int)GetLastError());
		assert(false);
		return false;
	}
	
	return exitCode == 0;
}

#else

bool DevEnv::detect()
{
	return false;
}

std::future<bool> DevEnv::build(const std::wstring& targetName)
{
	assert(false && "Not supported");
}

#endif
