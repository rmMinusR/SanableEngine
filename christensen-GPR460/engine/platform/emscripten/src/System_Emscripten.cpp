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

void gpr460::System_Emscripten::ShowError(const gpr460::string& message)
{
	jsnative_alert(message.c_str());
}

EM_JS(void, jsnative_console_error, (const wchar_t* message), {
	console.error(UTF16ToString(message));
})

void gpr460::System_Emscripten::LogToErrorFile(const gpr460::string& message)
{
	jsnative_console_error(message.c_str());
}

std::vector<std::filesystem::path> gpr460::System_Emscripten::ListPlugins(std::filesystem::path path) const
{
	std::vector<std::filesystem::path> contents;

	std::cout << "Reading plugins directory...\n";

	//*
	std::error_code err;
	auto it = std::filesystem::directory_iterator(path, err);
	if (err) std::cout << "ERROR reading plugins directory: code " << err.value() << ": " << err.message() << "\n";
	
	for (const std::filesystem::path& entry : std::filesystem::directory_iterator(path))
	{
		std::ostringstream joiner;
		std::cout << "Found " << entry.string() << "\n";
		joiner << entry.filename().string() << ".so"; //Build DLL name
		contents.push_back(entry / joiner.str());
	}
	// */

	/*
	std::ostringstream joiner;
	struct dirent *dir;
	DIR *d = opendir(path.c_str());
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			std::cout << "Found " << dir->d_name << "\n";
			if (dir->d_name[0] != '.')
			{
				joiner << dir->d_name << ".so"; //Build DLL name
				contents.push_back(path / dir->d_name / joiner.str());
				joiner.clear();
			}
		}
		closedir(d);
	}
	else std::cout << "ERROR: Could not read plugins directory\n";
	// */

	return contents;
}

std::filesystem::path gpr460::System_Emscripten::GetBaseDir() const
{
	return "/"; //FIXME?
}