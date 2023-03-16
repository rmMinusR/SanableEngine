#include "TTFModule.hpp"

#include <SDL_ttf.h>

void TTFModule::loadInternal(MemoryManager*)
{
	TTF_Init();
}

void TTFModule::unloadInternal(MemoryManager*)
{
	TTF_Quit();
}
