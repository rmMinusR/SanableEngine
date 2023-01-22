#include "System.hpp"

#include <SDL.h>

uint32_t GetTicks()
{
	return SDL_GetTicks();
}

void gpr460::System::Init(EngineCore* engine)
{
	this->engine = engine;
}