#include "System_Win32.hpp"

#ifdef _WIN32

//Memory leak tracing
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <cassert>
#include <sstream>

#include "EngineCore.hpp"

gpr460::System_Win32::System_Win32()
{
	consolePsuedofile = nullptr;
	logFile = nullptr;
}

gpr460::System_Win32::~System_Win32()
{
	assert(!isAlive);
}

void gpr460::System_Win32::Init(EngineCore* engine)
{
	System_Outline::Init(engine);

	assert(!isAlive);
	isAlive = true;

	//Log memory leaks
#ifdef _DEBUG
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtMemCheckpoint(&checkpoint);
#endif
	
	//Create console and redirect
	if (!AllocConsole()) ShowError(L"Failed to allocate console: Code %i", GetLastError());
	freopen_s(&consolePsuedofile, "CONOUT$", "w", stdout);
	if (!consolePsuedofile) ShowError(L"Failed to redirect console output");
}

void gpr460::System_Win32::DoMainLoop()
{
	while (true)
	{
		Uint32 now = GetTicks();
		if (now - engine->frameStart >= 16)
		{
			engine->frameStep(engine);
		}

		if (engine->quit)
			break;
	}
}

void gpr460::System_Win32::Shutdown()
{
	assert(isAlive);
	isAlive = false;

	//Close console redirection
	fclose(consolePsuedofile);
	consolePsuedofile = nullptr;

	if (logFile)
	{
		SetEndOfFile(logFile);
		CloseHandle(logFile);
		logFile = nullptr;
	}

	//_CrtDumpMemoryLeaks();
	_CrtMemDumpAllObjectsSince(&checkpoint);
}

void gpr460::System_Win32::ShowError(const gpr460::string& message)
{
	MessageBoxW(NULL, message.c_str(), L"Error", MB_OK | MB_ICONSTOP);
}

void gpr460::System_Win32::LogToErrorFile(const gpr460::string& message)
{
	//Lazy init logfile
	if (!logFile)
	{
		//TODO does this need FILE_FLAG_NO_BUFFERING or FILE_FLAG_WRITE_THROUGH?
		logFile = CreateFileW(logFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (!logFile || (GetLastError() != 0 && GetLastError() != ERROR_ALREADY_EXISTS)) ShowError(L"Failed to create error file (code %i)", GetLastError());
	}

	DWORD nWritten;
	if (!WriteFile(logFile, message.c_str(), message.length(), &nWritten, NULL))
	{
		ShowError(L"Error code %i while logging to file", GetLastError());
	}

	if (nWritten < message.length())
	{
		ShowError(L"Tried to write %u characters to error file, but only wrote %u", message.length(), nWritten);
	}
}

std::vector<std::filesystem::path> gpr460::System_Win32::ListPlugins(std::filesystem::path path) const
{
	std::vector<std::filesystem::path> contents;

	std::ostringstream joiner;
	for (const std::filesystem::path& entry : std::filesystem::directory_iterator(path))
	{
		joiner << entry.filename().string() << ".dll"; //Build DLL name
		contents.push_back(entry / joiner.str());
		joiner.clear();
	}

	return contents;
}

std::filesystem::path gpr460::System_Win32::GetBaseDir() const
{
	//Get host assembly
	constexpr size_t bufSz = 1024;
	wchar_t buf[bufSz];
	memset(buf, 0, sizeof(wchar_t)*bufSz);
	DWORD written = GetModuleFileName(NULL, buf, bufSz);
	assert(written != bufSz);

	//Search for last backslash
	long found;
	for (found = (long)written; found >= 0; --found)
	{
		if (buf[found] == L'\\') break;
	}
	assert(found != -1);

	//Substring
	buf[found] = L'\0';

	return std::wstring(buf);
}

#endif _WIN32