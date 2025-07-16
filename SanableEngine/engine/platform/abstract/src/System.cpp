#include "System.hpp"

void gpr460::System::Init(Application* engine)
{
	this->engine = engine;
	quitRequested = false;
}

gpr460::System::System()
{
	isAlive = false;
	engine = nullptr;
	targetFps = defaultTargetFps;
}

gpr460::System::~System()
{
}

void gpr460::System::requestQuit()
{
	quitRequested = true;
}
