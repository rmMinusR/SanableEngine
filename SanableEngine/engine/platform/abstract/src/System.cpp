#include "System.hpp"

#include "SDL_events.h"

void gpr460::System::Init()
{
	quitRequested = false;
}

gpr460::System::System()
{
	isAlive = false;
	targetFps = defaultTargetFps;
}

gpr460::System::~System()
{
}

void gpr460::System::requestQuit()
{
	quitRequested = true;
}
