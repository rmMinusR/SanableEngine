#pragma once

#include "dllapi.h"

#include "Aliases.hpp"

#include <vector>
#include <filesystem>

class EngineCore;
class PluginManager;

namespace gpr460
{

	class System
	{
	protected:
		bool isAlive = false;
		EngineCore* engine;
		float targetFps;
		static constexpr float defaultTargetFps = 60;

	public: //FIXME make protected again
		friend class EngineCore;
		ENGINECORE_API virtual void Init(EngineCore*);
		ENGINECORE_API virtual void DoMainLoop() = 0;
		ENGINECORE_API virtual void Shutdown() = 0;
		
		friend class PluginManager;
		virtual std::vector<std::filesystem::path> ListPlugins(std::filesystem::path path) const = 0;

	public:
		ENGINECORE_API System();
		virtual ~System() = default;

		virtual void DebugPause() = 0;

		//TODO these should be combined
		virtual void ShowError(const gpr460::string& message) = 0;
		virtual void LogToErrorFile(const gpr460::string& message) = 0;

		template<typename... Ts>
		void ShowErrorF(const gpr460::string& format, const Ts&... args)
		{
			constexpr size_t bufferSize = 256;
			wchar_t buffer[bufferSize];
			int nValid = swprintf_s(buffer, bufferSize, format.c_str(), args...);

			ShowError(gpr460::string(buffer, nValid));
		}

		virtual std::filesystem::path GetBaseDir() const = 0;
	};
}