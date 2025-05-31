#pragma once

#include <string>
#include <future>

namespace DevEnv
{
	bool detect();
	bool build(const std::wstring& targetName);
	std::future<bool> buildAsync(const std::wstring& targetName);
}
