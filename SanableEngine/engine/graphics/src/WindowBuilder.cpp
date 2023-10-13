#include "WindowBuilder.hpp"

#include "Window.hpp"
#include "EngineCore.hpp"

WindowBuilder::WindowBuilder(EngineCore* engine, const std::string& name, int width, int height, std::unique_ptr<WindowRenderPipeline>&& renderPipeline) :
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
	Window* window = new Window(name, size.x, size.y, engine, std::move(renderPipeline));
	if (position.has_value()) window->move(position.value().x, position.value().y);
	engine->windows.push_back(window);
	return window;
}

void WindowBuilder::destroy(Window* window)
{
	std::vector<Window*>& windows = window->engine->windows;
	windows.erase(std::find(windows.begin(), windows.end(), window));
	delete window;
}
