#pragma once

#include <string>
#include <memory>
#include <SDL_video.h>
#include <SDL_events.h>
#include <ThunkUtils.hpp>
#include <ReflectionSpec.hpp>
#include "../dllapi.h"
#include "Renderer.hpp"
#include "WindowRenderPipeline.hpp"

class WindowBuilder;
class ModuleTypeRegistry;
class WindowInputProcessor;
struct GLSettings;

class Window
{
private:
	SDL_Window* handle;
	SDL_GLContext context;
	Renderer _interface;
	WindowRenderPipeline* renderPipeline;
	WindowInputProcessor* inputProcessor;

	int sdlID;
	friend class WindowInputProcessor;
	bool closeRequested;

	Application* engine;
	friend class Application;
	void draw() const;
	void handleEvent(SDL_Event& ev);

	static Window* currentFocus;

	Window(const std::string& name, int width, int height, const GLSettings& glSettings, Application* engine, WindowRenderPipeline* renderPipeline, WindowInputProcessor* inputProcessor);
	friend class WindowBuilder;
	friend struct thunk_utils<Window>;
public:
	ENGINEGRAPHICS_API ~Window();

	ENGINEGRAPHICS_API void move(int x, int y);

	inline Renderer* getRenderer() { return &_interface; }
	inline Application* getEngine() { return engine; }

	ENGINEGRAPHICS_API int getWidth() const;
	ENGINEGRAPHICS_API int getHeight() const;
	ENGINEGRAPHICS_API bool isFocused() const;
	ENGINEGRAPHICS_API bool wasCloseRequested() const;

	inline WindowRenderPipeline* getRenderPipeline() { return renderPipeline; }
	inline WindowInputProcessor* getInputProcessor() { return inputProcessor; }

	ENGINEGRAPHICS_API static void setActiveDrawTarget(const Window* w);
};
