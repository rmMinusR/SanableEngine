#pragma once

#include <memory>
#include <SDL_video.h>
#include "dllapi.h"
#include "Renderer.hpp"

struct SDL_Window;
struct SDL_Renderer;
class WindowRenderPipeline;

class Window
{
private:
	SDL_Window* handle;
	SDL_GLContext context;
	Renderer _interface;
	std::shared_ptr<WindowRenderPipeline> renderPipeline;

	friend class EngineCore;
	void draw() const;
public:
	ENGINEGRAPHICS_API Window(char const* name, int width, int height, EngineCore* engine, std::shared_ptr<WindowRenderPipeline> renderPipeline);
	ENGINEGRAPHICS_API ~Window();

	inline Renderer* getRenderer() { return &_interface; }

	ENGINEGRAPHICS_API int getWidth() const;
	ENGINEGRAPHICS_API int getHeight() const;
};
