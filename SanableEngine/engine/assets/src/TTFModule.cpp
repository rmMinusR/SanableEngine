#include "TTFModule.hpp"

#include <SDL_ttf.h>

void TTFModule::loadInternal()
{
	TTF_Init();
}

void TTFModule::unloadInternal()
{
	TTF_Quit();
}
