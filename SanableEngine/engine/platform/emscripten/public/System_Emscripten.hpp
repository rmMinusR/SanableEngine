#pragma once

#include "System.hpp"

namespace gpr460
{
	class System_Emscripten : public System
	{
	public:
		System_Emscripten();
		~System_Emscripten();

		void Init() override;
		void DoMainLoop(void(*stepFn)(Application*), Application* app) override;
		void Shutdown() override;

		void DebugPause() override;

		void ShowError(const std::wstring& message) override;
		void LogToErrorFile(const std::wstring& message) override;

		std::vector<std::filesystem::path> ListPlugins(std::filesystem::path path) const override;
		std::filesystem::path GetBaseDir() const override;
	};
}
