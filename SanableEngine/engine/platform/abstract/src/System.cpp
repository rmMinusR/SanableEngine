#include "System.hpp"

#include <cassert>
#include <GL/glew.h>

void gpr460::System::Init(EngineCore* engine)
{
	this->engine = engine;
}

gpr460::System::System()
{
	isAlive = false;
	engine = nullptr;
	targetFps = defaultTargetFps;

	GLenum err = glewInit();
	assert(!err);
}

gpr460::System::~System()
{
}