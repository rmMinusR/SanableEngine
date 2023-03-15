#include "SDLModule.hpp"

#include <cassert>

void SDLModule::loadInternal(MemoryManager*)
{
	int err = SDL_InitSubSystem(flags);
	assert(!err);
}

void SDLModule::unloadInternal(MemoryManager*)
{
	SDL_QuitSubSystem(flags);
}

SDLModule::SDLModule(Uint32 flags) :
	flags(flags)
{
}

SDLModule SDLModule::timer         (SDL_INIT_TIMER         );
SDLModule SDLModule::audio         (SDL_INIT_AUDIO         );
SDLModule SDLModule::video         (SDL_INIT_VIDEO         );
SDLModule SDLModule::joystick      (SDL_INIT_JOYSTICK      );
SDLModule SDLModule::haptic        (SDL_INIT_HAPTIC        );
SDLModule SDLModule::gamecontroller(SDL_INIT_GAMECONTROLLER);
SDLModule SDLModule::events        (SDL_INIT_EVENTS        );
