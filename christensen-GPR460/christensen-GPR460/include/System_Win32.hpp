#pragma once

#ifdef _WIN32

#include "System_Outline.hpp"

#include <Windows.h>

//Memory leak tracing
#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

namespace gpr460
{

	class System_Win32 : System_Outline
	{
	private:
		FILE* consolePsuedofile;
		HANDLE logFile;
		const string logFileName = TEXT("GameErrors.txt");

#ifdef _DEBUG
		_CrtMemState checkpoint;
#endif

	public:
		System_Win32();
		~System_Win32();

		void Init(EngineCore*) override;
		void DoMainLoop() override;
		void Shutdown() override;

		void ShowError(const gpr460::string& message) override;
		void LogToErrorFile(const gpr460::string& message) override;

		template<typename... Ts>
		void ShowError(const gpr460::string& format, const Ts&... args)
		{
			constexpr size_t bufferSize = 256;
			wchar_t buffer[bufferSize];
			int nValid = swprintf_s(buffer, bufferSize, format.c_str(), args...);

			ShowError(gpr460::string(buffer, nValid));
		}

		std::vector<std::filesystem::path> ListPlugins(std::filesystem::path path) const override;
		std::filesystem::path GetBaseDir() const override;
	};

}

#endif _WIN32