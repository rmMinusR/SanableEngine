#pragma once

#include <unordered_map>
#include <SDL_video.h>
#include "dllapi.h"

class GLContext
{
	static std::unordered_map<SDL_GLContext, int> handles; //ptr -> refcount
	static bool glewGood;

public:
	ENGINEOPENGL_API static SDL_GLContext create(SDL_Window* window, void* who);
	ENGINEOPENGL_API static void release(SDL_GLContext ctx, void* who);
};
