#include <iostream>

#include "Logger.h"
#include "SDL.h"

SDLMAIN_DECLSPEC int SDL_main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	Logger::debug("Hello, world!");
	Logger::info("Hello, world!");
	Logger::warn("Hello, world!");
	Logger::error("Hello, world!");
	Logger::fatal("Hello, world!");

	return 0;
}