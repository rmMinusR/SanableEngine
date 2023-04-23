#include "System_Emscripten.hpp"

#include <emscripten.h>

#include <dirent.h>

#include <iostream>
#include <cassert>
#include <sstream>

#include "EngineCore.hpp"

gpr460::System_Emscripten::System_Emscripten()
{
}

gpr460::System_Emscripten::~System_Emscripten()
{
	assert(!isAlive);
}

void gpr460::System_Emscripten::DebugPause()
{
#ifdef _DEBUG
	//Pause so we can read console
	wprintf(L"\nDEBUG Paused, type any key in console to continue\n");
	std::cin.get();
#endif
}

void gpr460::System_Emscripten::Init(EngineCore* engine)
{
	System::Init(engine);

	assert(!isAlive);
	isAlive = true;
}

void gpr460::System_Emscripten::DoMainLoop()
{
	emscripten_set_main_loop_arg(EngineCore::frameStep, engine, 0, true);
}

void gpr460::System_Emscripten::Shutdown()
{
	assert(isAlive);
	isAlive = false;
}

EM_JS(void, jsnative_alert, (const wchar_t* message), {
	alert(UTF16ToString(message));
})

void gpr460::System_Emscripten::ShowError(const std::wstring& message)
{
	jsnative_alert(message.c_str());
}

EM_JS(void, jsnative_console_error, (const wchar_t* message), {
	console.error(UTF16ToString(message));
})

void gpr460::System_Emscripten::LogToErrorFile(const std::wstring& message)
{
	jsnative_console_error(message.c_str());
}

std::vector<std::filesystem::path> gpr460::System_Emscripten::ListPlugins(std::filesystem::path path) const
{
	std::vector<std::filesystem::path> contents;

	std::cout << "Reading plugins directory...\n";

	if (!std::filesystem::exists(path))
	{
		printf("Plugins folder does not exist, creating\n");
		std::filesystem::create_directory(path);
		return contents;
	}

	for (const std::filesystem::path& entry : std::filesystem::directory_iterator(path))
	{
		std::ostringstream joiner;
		std::cout << "Found " << entry.string() << "\n";
		joiner << entry.filename().string() << PLATFORM_DLL_EXTENSION; //Build DLL name
		contents.push_back(entry / joiner.str());
	}

	return contents;
}

std::filesystem::path gpr460::System_Emscripten::GetBaseDir() const
{
	return "/";
}