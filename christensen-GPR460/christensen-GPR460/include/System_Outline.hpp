#pragma once

#include "Aliases.hpp"

class EngineCore;

namespace gpr460
{

	class System_Outline
	{
	protected:
		bool isAlive = false;
		EngineCore* engine;

	public:
		System_Outline() = default;
		virtual ~System_Outline() = default;

		virtual void Init(EngineCore*);
		virtual void DoMainLoop() = 0;
		virtual void Shutdown() = 0;

		//TODO these should be combined
		virtual void ShowError(const gpr460::string& message) = 0;
		virtual void LogToErrorFile(const gpr460::string& message) = 0;

		template<typename... Ts>
		void ShowError(const gpr460::string& format, const Ts&... args)
		{
			constexpr size_t bufferSize = 256;
			wchar_t buffer[bufferSize];
			int nValid = swprintf_s(buffer, bufferSize, format.c_str(), args...);

			ShowError(gpr460::string(buffer, nValid));
		}
	};
}