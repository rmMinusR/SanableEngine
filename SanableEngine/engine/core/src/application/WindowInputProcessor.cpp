#include "application/WindowInputProcessor.hpp"

WindowInputProcessor::WindowInputProcessor() :
	window(nullptr)
{
}

WindowInputProcessor::~WindowInputProcessor()
{
}

void WindowInputProcessor::setup(Window* window)
{
	this->window = window;
}
