#pragma once

#include <string>
#include <vector>
#include <filesystem>

class Application;
class PluginManager;
class Window;
struct WindowSettings;

namespace gpr460
{

	class System
	{
	protected:
		bool isAlive = false;
		bool quitRequested = false;
		float targetFps;
		static constexpr float defaultTargetFps = 60;

	public: //FIXME make protected again
		friend class ::PluginManager;
		virtual std::vector<std::filesystem::path> ListPlugins(std::filesystem::path path) const = 0;

	public:
		System();
		virtual ~System();

		virtual void Init();
		virtual void DoMainLoop(void(*stepFn)(void*), void* stepArg) = 0;
		virtual void Shutdown() = 0;

		virtual void pumpEvents() = 0;

		virtual bool isFocused(const Window*) = 0;
		virtual Window* createWindow(const WindowSettings& settings, Application* engine) = 0;
		virtual void destroyWindow(Window* window) = 0;
		virtual size_t getNumWindows() const = 0;
		virtual Window* getWindow(size_t which) = 0;

		void requestQuit();

		virtual void DebugPause() = 0;

		//TODO these should be combined
		virtual void ShowError(const std::wstring& message) = 0;
		virtual void LogToErrorFile(const std::wstring& message) = 0;

		template<typename... Ts>
		void ShowErrorF(const std::wstring& format, const Ts&... args)
		{
			constexpr size_t bufferSize = 256;
			wchar_t buffer[bufferSize];
			int nValid = swprintf_s(buffer, bufferSize, format.c_str(), args...);

			ShowError(std::wstring(buffer, nValid));
		}

		virtual std::filesystem::path GetBaseDir() const = 0;
	};
}