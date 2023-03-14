#pragma once

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#include "System.hpp"

#include <Windows.h>

//Memory leak tracing
#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

class EngineCore;

namespace gpr460
{

	class System_Win32 : public System
	{
	private:
		FILE* consolePsuedofile;
		HANDLE logFile;
		const string logFileName = L"GameErrors.txt";

		friend class EngineCore;
#ifdef _DEBUG
		_CrtMemState checkpoint;
#endif
	protected:
		void Init(EngineCore*) override;
		void DoMainLoop() override;
		void Shutdown() override;

	public:
		System_Win32();
		~System_Win32();

		void DebugPause() override;

		void ShowError(const gpr460::string& message) override;
		void LogToErrorFile(const gpr460::string& message) override;

		std::vector<std::filesystem::path> ListPlugins(std::filesystem::path path) const override;
		std::filesystem::path GetBaseDir() const override;
	};

}
