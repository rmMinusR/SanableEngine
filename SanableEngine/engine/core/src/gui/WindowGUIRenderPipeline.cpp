#include "gui/WindowGUIRenderPipeline.hpp"

#include <GL/glew.h>
#include "application/Window.hpp"

WindowGUIRenderPipeline::WindowGUIRenderPipeline()
{
}

WindowGUIRenderPipeline::~WindowGUIRenderPipeline()
{
}

void WindowGUIRenderPipeline::render(Rect<float> viewport)
{
	//Set flags
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//Setup matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, viewport.width, viewport.height, 0, zNear, zFar); //+Y is down

	//Clear screen
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Render GUI
	hud.render(window->getRenderer());
}
