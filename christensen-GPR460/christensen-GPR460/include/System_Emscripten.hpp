#pragma once

#include "System_Outline.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace gpr460
{

	class System_Emscripten : System_Outline
	{
	private:
		bool isAlive;

	public:
		System_Emscripten();
		~System_Emscripten();

		void Init();
		void Shutdown();

		//TODO these should be combined
		void ShowError(const gpr460::string& message);
		void LogToErrorFile(const gpr460::string& message);

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