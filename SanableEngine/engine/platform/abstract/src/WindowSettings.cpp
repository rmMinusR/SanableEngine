#include "WindowSettings.hpp"

#include <cassert>

WindowSettings::WindowSettings(const std::string& name, int width, int height) :
	name(name),
	size(width, height),
	renderPipeline(nullptr),
	inputProcessor(nullptr)
{
}

WindowSettings::~WindowSettings()
{
	assert(!renderPipeline && "Resource leak!");
	assert(!inputProcessor && "Resource leak!");
}
