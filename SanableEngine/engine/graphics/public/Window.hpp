#pragma once

#include <SDL_video.h>
#include "dllapi.h"
#include "Renderer.hpp"

struct SDL_Window;
struct SDL_Renderer;

class Window
{
private:
	SDL_Window* handle;
	SDL_GLContext context;
	Renderer _interface;

	friend class Renderer;
public:
	ENGINEGRAPHICS_API Window(char const* name, int width, int height);
	ENGINEGRAPHICS_API ~Window();

	inline Renderer* getRenderer() { return &_interface; }

	ENGINEGRAPHICS_API int getWidth() const;
	ENGINEGRAPHICS_API int getHeight() const;
};
