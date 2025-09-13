#include "System_Win32.hpp"

//Memory leak tracing
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>

#include <SDL.h>

#include "GLContext.hpp"

#include "WindowSettings.hpp"
#include "Window_Win32.hpp"

gpr460::System_Win32::System_Win32(GLSettings glSettings) :
	glSettings(glSettings)
{
	consolePsuedofile = nullptr;
	logFile = nullptr;
#if _DEBUG
	memset(&checkpoint, 0, sizeof(_CrtMemState));
#endif
}

gpr460::System_Win32::~System_Win32()
{
	assert(!isAlive);
	assert(windows.empty());
}

void gpr460::System_Win32::Init()
{
	System::Init();

	assert(!isAlive);
	isAlive = true;

	//Log memory leaks
#ifdef _DEBUG
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtMemCheckpoint(&checkpoint);
#endif
	
	//Create console and redirect
	if (!AllocConsole()) System::ShowErrorF(L"Failed to allocate console: Code %i", GetLastError());
	freopen_s(&consolePsuedofile, "CONOUT$", "w", stdout);
	if (!consolePsuedofile) ShowError(L"Failed to redirect console output");
}

void gpr460::System_Win32::DoMainLoop(void(*stepFn)(void*), void* stepArg)
{
	while (true)
	{
		std::chrono::time_point frameStart = std::chrono::steady_clock::now();
		
		stepFn(stepArg);
		if (quitRequested) break;

		std::chrono::time_point frameEnd = std::chrono::steady_clock::now();

		std::chrono::duration<float> elapsed = frameEnd - frameStart;
		std::chrono::duration<float> targetTimePerFrame = std::chrono::duration<float>{ 1 } / targetFps;
		if (elapsed < targetTimePerFrame) Sleep(std::chrono::duration_cast<std::chrono::milliseconds>(targetTimePerFrame - elapsed).count());
	}
}

void gpr460::System_Win32::Shutdown()
{
	assert(isAlive);
	isAlive = false;

	for (auto w = windows.rbegin(); w != windows.rend(); ++w) delete *w;
	windows.clear();

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

void gpr460::System_Win32::DebugPause()
{
#ifdef _DEBUG
	//Pause so we can read console
	WriteConsoleW(logFile, L"\nDEBUG Paused, type any key in console to continue\n", 52, nullptr, nullptr);
	std::cin.get();
#endif
}

void gpr460::System_Win32::ShowError(const std::wstring& message)
{
	MessageBoxW(NULL, message.c_str(), L"Error", MB_OK | MB_ICONSTOP);
}

void gpr460::System_Win32::LogToErrorFile(const std::wstring& message)
{
	//Lazy init logfile
	if (!logFile)
	{
		//TODO does this need FILE_FLAG_NO_BUFFERING or FILE_FLAG_WRITE_THROUGH?
		logFile = CreateFileW(logFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (!logFile || (GetLastError() != 0 && GetLastError() != ERROR_ALREADY_EXISTS)) System::ShowErrorF(L"Failed to create error file (code %i)", GetLastError());
	}

	DWORD nWritten;
	if (!WriteFile(logFile, message.c_str(), (DWORD)message.length(), &nWritten, NULL))
	{
		System::ShowErrorF(L"Error code %i while logging to file", GetLastError());
	}

	if (nWritten < message.length())
	{
		System::ShowErrorF(L"Tried to write %u characters to error file, but only wrote %u", message.length(), nWritten);
	}
}

std::vector<std::filesystem::path> gpr460::System_Win32::ListPlugins(std::filesystem::path path) const
{
	std::vector<std::filesystem::path> contents;

	if (!std::filesystem::exists(path))
	{
		printf("Plugins folder does not exist, creating\n");
		std::filesystem::create_directory(path);
	}

	for (const std::filesystem::path& entry : std::filesystem::directory_iterator(path))
	{
		contents.push_back(entry);
	}

	return contents;
}

std::filesystem::path gpr460::System_Win32::GetBaseDir() const
{
	//Get host assembly
	constexpr size_t bufSz = 1024;
	wchar_t buf[bufSz];
	memset(buf, 0, sizeof(wchar_t)*bufSz);
	DWORD written = GetModuleFileNameW(NULL, buf, bufSz);
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

void gpr460::System_Win32::pumpEvents()
{
	assert(isAlive);

	SDL_Event event;

	//Utility functions
	auto lookupWindow = [&](SDL_Window* windowHandle)
	{
		auto it = std::find_if(windows.begin(), windows.end(), [=](Window_Win32* w) { return w->getSdlHandle() == windowHandle; });
		if (it != windows.end()) return *it;
		else return (Window_Win32*)nullptr;
	};
	auto forwardToWindow = [&](SDL_Window* windowHandle)
	{
		Window_Win32* w = lookupWindow(windowHandle);
		if (w) (w)->handleEvent(event);
	};

	while (SDL_PollEvent(&event))
	{
		//Forward events to appropriate windows
		switch (event.type)
		{
			// Passthrough input
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			forwardToWindow(SDL_GetKeyboardFocus());
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEMOTION:
			forwardToWindow(SDL_GetMouseFocus());
			break;

			//Window focus management
		case SDL_WINDOWEVENT:
		{
			Window_Win32* window = lookupWindow(SDL_GetWindowFromID(event.window.windowID));
			if (window) // Null if events happened before destruction but after last pump
			{
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					currentFocus = window;
					break;

				case SDL_WINDOWEVENT_FOCUS_LOST:
					if (currentFocus == window) currentFocus = nullptr;
					break;

				case SDL_WINDOWEVENT_CLOSE:
					window->requestClose();
					break;
				}
				window->handleEvent(event);
				break;
			}
		}

		}
	}
}

bool gpr460::System_Win32::isFocused(const Window* window)
{
	return window == currentFocus;
}

Window* gpr460::System_Win32::createWindow(const WindowSettings& settings, Application* engine)
{
	SDL_InitSubSystem(SDL_INIT_VIDEO); //Internally refcounted, no checks necessary
	SDL_SetWindowsMessageHook(
		+[](void* userdata, void* hWnd, unsigned int message, Uint64 wParam, Sint64 lParam)
		{
			static_cast<System_Win32*>(userdata)->handleNativeEvent((HWND)hWnd, message, wParam, lParam);
		},
		this
	);

	glSettings.apply();

	SDL_Window* handle = SDL_CreateWindow(
		settings.name.c_str(),
		settings.position.has_value() ? settings.position->x : SDL_WINDOWPOS_UNDEFINED,
		settings.position.has_value() ? settings.position->y : SDL_WINDOWPOS_UNDEFINED,
		settings.size.x,
		settings.size.y,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | (settings.resizable ? SDL_WINDOW_RESIZABLE : 0)
	);

	Window_Win32* w = new Window_Win32(settings, engine, handle);
	windows.push_back(w);
	return w;
}

void gpr460::System_Win32::destroyWindow(Window* _window)
{
	Window_Win32* window = static_cast<Window_Win32*>(_window);

	delete window;

	auto it = std::find(windows.begin(), windows.end(), window);
	assert(it != windows.end());
	windows.erase(it);
}

size_t gpr460::System_Win32::getNumWindows() const
{
	return windows.size();
}

Window* gpr460::System_Win32::getWindow(size_t which)
{
	return windows[which];
}

void gpr460::System_Win32::handleNativeEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	for (Window_Win32* w : windows)
	{
		if (w->getNativeHandle() == hwnd) w->handleNativeEvent(uMsg, wParam, lParam);
	}
}
