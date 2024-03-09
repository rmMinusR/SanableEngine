#include "application/WindowBuilder.hpp"

#include "application/Window.hpp"
#include "application/Application.hpp"

void WindowBuilder::setInputProcessor(std::unique_ptr<WindowInputProcessor>&& inputProcessor)
{
	this->inputProcessor = std::move(inputProcessor);
}

WindowBuilder::WindowBuilder(Application* engine, const std::string& name, int width, int height, const GLSettings& glSettings, std::unique_ptr<WindowRenderPipeline>&& renderPipeline) :
	glSettings(glSettings),
	engine(engine),
	name(name),
	size(width, height, 0),
	renderPipeline(std::move(renderPipeline))
{
}

WindowBuilder::~WindowBuilder()
{
}

Window* WindowBuilder::build()
{
	Window* window = new Window(name, size.x, size.y, glSettings, engine, std::move(renderPipeline), std::move(inputProcessor));
	if (position.has_value()) window->move(position.value().x, position.value().y);
	engine->windows.push_back(window);
	window->renderPipeline->setup(window);
	return window;
}

void WindowBuilder::destroy(Window* window)
{
	std::vector<Window*>& windows = window->engine->windows;
	windows.erase(std::find(windows.begin(), windows.end(), window));
	delete window;
}
