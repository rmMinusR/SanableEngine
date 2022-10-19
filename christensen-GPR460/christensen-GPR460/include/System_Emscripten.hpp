#pragma once

#ifdef _WIN32
#include "../../emsdk/upstream/emscripten/cache/sysroot/include/emscripten.h"
#else
#include <emscripten.h>
#endif

#include "System_Outline.hpp"

namespace gpr460
{

	class System_Emscripten : System_Outline
	{
	public:
		System_Emscripten();
		~System_Emscripten();

		void Init() override;
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
	};

}