#include "System.hpp"

#include <SDL.h>

void gpr460::System::Init(EngineCore* engine)
{
	this->engine = engine;
}

gpr460::System::System()
{
	isAlive = false;
	engine = nullptr;
	targetFps = defaultTargetFps;
}