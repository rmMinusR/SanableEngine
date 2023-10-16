#include "application/WindowRenderPipeline.hpp"

WindowRenderPipeline::WindowRenderPipeline() :
	window(nullptr)
{
}

WindowRenderPipeline::~WindowRenderPipeline()
{
}

void WindowRenderPipeline::setup(Window* window)
{
	this->window = window;
}
