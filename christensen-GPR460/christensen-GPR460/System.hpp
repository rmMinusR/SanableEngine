#pragma once

#include <Windows.h>

#include "Aliases.hpp"

namespace gpr460
{

	class System
	{
	private:
		bool isAlive;
		FILE* consolePsuedofile;

	public:
		System();
		~System();

		void Init();
		void Shutdown();

		//TODO these should be combined
		void ShowError(const gpr460::string& message);
		void LogToErrorFile(const gpr460::string& message);

	};

}


