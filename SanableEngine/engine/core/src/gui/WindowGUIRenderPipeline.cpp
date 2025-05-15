#include "gui/WindowGUIRenderPipeline.hpp"

#include <GL/glew.h>
#include "Window.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"

WindowGUIRenderPipeline::WindowGUIRenderPipeline(Application* application) :
	hud(application)
{
}

WindowGUIRenderPipeline::~WindowGUIRenderPipeline()
{
}

void WindowGUIRenderPipeline::render(Rect<float> viewport)
{
	Renderer* renderer = window->getRenderer();

	Camera cam;
	cam.setGUIProj();
	renderer->beginFrame(cam, viewport, {0,0,0}, glm::identity<glm::quat>());

	renderer->clear({0, 0, 0, 1});

	//Tick and render GUI
	hud.refreshLayout(viewport);
	hud.tick();
	hud.render(window->getRenderer());
}
