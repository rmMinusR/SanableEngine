#include "System.hpp"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <cassert>
#include <Windows.h>

gpr460::System::System()
{
	isAlive = false;
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

}

void gpr460::System::Shutdown()
{
	assert(isAlive);
	isAlive = false;

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
