#include "System_Win32.hpp"

#include <cassert>

//Memory leak tracing
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

gpr460::System_Win32::System_Win32()
{
	isAlive = false;

	consolePsuedofile = nullptr;
	logFile = nullptr;
}

gpr460::System_Win32::~System_Win32()
{
	assert(!isAlive);
}

void gpr460::System_Win32::Init()
{
	assert(!isAlive);
	isAlive = true;

	//Log memory leaks
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif
	
	//Create console and redirect
	if (!AllocConsole()) ShowError(TEXT("Failed to allocate console"));
	freopen_s(&consolePsuedofile, "CONOUT$", "w", stdout);
	if (!consolePsuedofile) ShowError(TEXT("Failed to redirect console output"));
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

	_CrtDumpMemoryLeaks();
}

void gpr460::System_Win32::ShowError(const gpr460::string& message)
{
	MessageBox(NULL, message.c_str(), TEXT("Error"), MB_OK | MB_ICONSTOP);
}

void gpr460::System_Win32::LogToErrorFile(const gpr460::string& message)
{
	//Lazy init logfile
	if (!logFile)
	{
		//TODO does this need FILE_FLAG_NO_BUFFERING or FILE_FLAG_WRITE_THROUGH?
		logFile = CreateFile(logFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (!logFile || (GetLastError() != 0 && GetLastError() != ERROR_ALREADY_EXISTS)) ShowError(TEXT("Failed to create error file (code %i)"), GetLastError());
	}

	DWORD nWritten;
	if (!WriteFile(logFile, message.c_str(), message.length(), &nWritten, NULL))
	{
		ShowError(TEXT("Error code %i while logging to file"), GetLastError());
	}

	if (nWritten < message.length())
	{
		ShowError(TEXT("Tried to write %u characters to error file, but only wrote %u"), message.length(), nWritten);
	}
}
