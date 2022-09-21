#include "System.hpp"

#include <cassert>

//Memory leak tracing
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

gpr460::System::System()
{
	isAlive = false;
	consolePsuedofile = nullptr;
}

gpr460::System::~System()
{
	assert(!isAlive);
}

void gpr460::System::Init()
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

void gpr460::System::Shutdown()
{
	assert(isAlive);
	isAlive = false;

	//Close console redirection
	fclose(consolePsuedofile);
	consolePsuedofile = nullptr;

	_CrtDumpMemoryLeaks();
}

void gpr460::System::ShowError(const gpr460::string& message)
{
	MessageBox(NULL, message.c_str(), TEXT("Error"), MB_OK | MB_ICONSTOP);
}

void gpr460::System::LogToErrorFile(const gpr460::string& message)
{
	//TODO IMPL
}
