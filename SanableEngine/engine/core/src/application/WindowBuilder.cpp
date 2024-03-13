#include "application/WindowBuilder.hpp"

#include "application/Window.hpp"
#include "application/Application.hpp"

WindowBuilder::WindowBuilder(Application* engine, const std::string& name, int width, int height, const GLSettings& glSettings, WindowRenderPipeline* renderPipeline) :
	glSettings(glSettings),
	engine(engine),
	name(name),
	size(width, height, 0),
	renderPipeline(renderPipeline),
	inputProcessor(nullptr)
{
}

WindowBuilder::~WindowBuilder()
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

void WindowBuilder::setInputProcessor(WindowInputProcessor* inputProcessor)
{
	if (this->inputProcessor) delete this->inputProcessor;

	this->inputProcessor = inputProcessor;
}

Window* WindowBuilder::build()
{
	Window* window = new Window(name, size.x, size.y, glSettings, engine, renderPipeline, inputProcessor);
	if (position.has_value()) window->move(position.value().x, position.value().y);
	engine->windows.push_back(window);
	window->renderPipeline->setup(window);
	if (window->inputProcessor) window->inputProcessor->setup(window);

	renderPipeline = nullptr;
	inputProcessor = nullptr;

	return window;
}
