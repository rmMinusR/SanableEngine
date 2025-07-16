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

#include "GLSettings.hpp"

class Application;
class Window_Win32;

namespace gpr460
{

	class System_Win32 : public System
	{
	private:
		FILE* consolePsuedofile;
		HANDLE logFile;
		const std::wstring logFileName = L"GameErrors.txt";

		GLSettings glSettings;

		Window_Win32* currentFocus = nullptr;
		std::vector<Window_Win32*> windows;

		friend class ::Application;
#ifdef _DEBUG
		_CrtMemState checkpoint;
#endif
	protected:
		void Init(Application*) override;
		void DoMainLoop(void(*stepFn)(void*), void* stepArg) override;
		void Shutdown() override;

	public:
		System_Win32(GLSettings);
		~System_Win32();

		virtual void pumpEvents() override;

		bool isFocused(const Window*) override;
		Window* createWindow(const WindowSettings& settings, Application* engine) override;
		void destroyWindow(Window* window) override;
		size_t getNumWindows() const override;
		Window* getWindow(size_t which) override;

		void DebugPause() override;

		void ShowError(const std::wstring& message) override;
		void LogToErrorFile(const std::wstring& message) override;

		std::vector<std::filesystem::path> ListPlugins(std::filesystem::path path) const override;
		std::filesystem::path GetBaseDir() const override;

	protected:
		void handleNativeEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};

}
