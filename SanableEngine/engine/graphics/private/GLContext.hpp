#pragma once

#include <unordered_map>
#include <SDL_video.h>

class GLContext
{
	static std::unordered_map<SDL_GLContext, int> handles; //ptr -> refcount
	static bool glewGood;

public:
	static SDL_GLContext create(SDL_Window* window, void* who);
	static void release(SDL_GLContext ctx, void* who);
};
