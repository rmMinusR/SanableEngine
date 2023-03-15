#pragma once

#include <SDL.h>

#include "Asset.hpp"
#include "dllapi.h"

class SDLModule : public Asset
{
protected:
	virtual void loadInternal(MemoryManager*) override;
	virtual void unloadInternal(MemoryManager*) override;

	Uint32 flags;
public:
	SDLModule(Uint32 flags);

	//Default SDL modules
	ENGINEASSETS_API static SDLModule timer;
	ENGINEASSETS_API static SDLModule audio;
	ENGINEASSETS_API static SDLModule video;
	ENGINEASSETS_API static SDLModule joystick;
	ENGINEASSETS_API static SDLModule haptic;
	ENGINEASSETS_API static SDLModule gamecontroller;
	ENGINEASSETS_API static SDLModule events;
};
