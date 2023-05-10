#pragma once

#include <vector>
#include <filesystem>

class EngineCore;
class PluginManager;

int main(int argc, char* argv[]);
int SDL_main(int argc, char* argv[]);

namespace gpr460
{

	class System
	{
	protected:
		bool isAlive = false;
		EngineCore* engine;
		float targetFps;
		static constexpr float defaultTargetFps = 60;

	protected:
		friend class ::EngineCore;
		virtual void Init(EngineCore*);
		virtual void DoMainLoop() = 0;
		virtual void Shutdown() = 0;
		
		friend class ::PluginManager;
		virtual std::vector<std::filesystem::path> ListPlugins(std::filesystem::path path) const = 0;

	public:
		System();
		virtual ~System();

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