#include "System_Emscripten.hpp"

#ifdef __EMSCRIPTEN__

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

void gpr460::System_Emscripten::Init(EngineCore* engine)
{
	System_Outline::Init(engine);

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

//extern "C" void jsnative_alert(const wchar_t* message);

void gpr460::System_Emscripten::ShowError(const gpr460::string& message)
{
	//jsnative_alert(message.c_str());

	//constexpr size_t bufferSize = 256;
	//wchar_t buffer[bufferSize];
	//memset(buffer, '\0', bufferSize);
	//int nValid = swprintf_s(buffer, bufferSize, L"alert($%s)", message.c_str());
	
	//emscripten_run_script(buffer);
	//EM_ASM({
	//	alert($0);
	//}, message);
}

//extern "C" void jsnative_console_error(const wchar_t* message);

void gpr460::System_Emscripten::LogToErrorFile(const gpr460::string& message)
{
	//jsnative_console_error(message.c_str());

	//EM_ASM({
	//	console.log($0);
	//}, message);
}

std::vector<std::filesystem::path> gpr460::System_Emscripten::ListPlugins(std::filesystem::path path) const
{
	std::vector<std::filesystem::path> contents;

	std::cout << "Reading plugins directory...\n";

	//*
	std::error_code err;
	auto it = std::filesystem::directory_iterator(path, err);
	if (err) std::cout << "ERROR reading plugins directory: code " << err.value() << ": " << err.message() << "\n";
	
	std::ostringstream joiner;
	for (const std::filesystem::path& entry : std::filesystem::directory_iterator(path))
	{
		std::cout << "Found " << entry.string() << "\n";
		joiner << entry.filename().string() << ".so"; //Build DLL name
		contents.push_back(entry / joiner.str());
		joiner.clear();
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

std::filesystem::path gpr460::System_Win32::GetBaseDir() const
{
	return "/"; //FIXME?
}

#endif __EMSCRIPTEN__