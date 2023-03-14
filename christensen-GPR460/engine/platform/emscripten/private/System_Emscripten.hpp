#pragma once

#include "System.hpp"

namespace gpr460
{
	class System_Emscripten : public System
	{
	protected:
		friend class EngineCore;
		void Init(EngineCore*) override;
		void DoMainLoop() override;
		void Shutdown() override;

	public:
		System_Emscripten();
		~System_Emscripten();

		void DebugPause() override;

		void ShowError(const gpr460::string& message) override;
		void LogToErrorFile(const gpr460::string& message) override;

		std::vector<std::filesystem::path> ListPlugins(std::filesystem::path path) const override;
		std::filesystem::path GetBaseDir() const override;
	};
}
