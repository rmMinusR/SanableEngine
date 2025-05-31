#include "WindowSettings.hpp"

#include <cassert>

WindowSettings::WindowSettings(const std::string& name, int width, int height) :
	name(name),
	size(width, height),
	userLogic(nullptr)
{
}

WindowSettings::~WindowSettings()
{
	assert(!userLogic && "Resource leak!");
}
