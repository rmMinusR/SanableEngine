#pragma once

#include <SDL_video.h>

namespace GLContext
{
	SDL_GLContext create(SDL_Window* window, void* who);
	void release(SDL_GLContext ctx, void* who);
};
