#include "application/WindowSettings.hpp"

#include "application/WindowInputProcessor.hpp"
#include "application/WindowRenderPipeline.hpp"

WindowSettings::WindowSettings(const std::string& name, int width, int height) :
	name(name),
	size(width, height),
	renderPipeline(nullptr),
	inputProcessor(nullptr)
{
}

WindowSettings::~WindowSettings()
{
	if (renderPipeline)
	{
		delete renderPipeline;
		renderPipeline = nullptr;
	}

	if (inputProcessor)
	{
		delete inputProcessor;
		inputProcessor = nullptr;
	}
}
