#include "DevEnv.hpp"

#include <cassert>
#include <filesystem>
#include <sstream>

#include "application/Plugin.hpp"
#include "game/Game.hpp"
#include "System.hpp"

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
	return std::async(build, targetName);
}

bool DevEnv::build(const std::wstring& targetName)
{
	STARTUPINFOW startup;
	startup.cb = sizeof(startup);

	std::wostringstream cliExpr;
	cliExpr << L"cmake --build . -t \"" << targetName << L'"';

	PROCESS_INFORMATION proc;
	bool ok = CreateProcessW(
		cliExpr.str().c_str(),
		NULL,
		NULL, // proc security
		NULL, // thread security
		FALSE,
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
		NULL, // env vars
		game->getApplication()->getSystem()->GetBaseDir().wstring().c_str(),
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
