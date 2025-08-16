#include "Window.hpp"

#include "System.hpp"
#include "WindowSettings.hpp"
#include "WindowUserLogic.hpp"

void Window::handleEvent(const SDL_Event& ev)
{
	if (userLogic) userLogic->handleEvent(ev);
}

Window::Window(const WindowSettings& settings, gpr460::System* system, void* context) :
	system(system),
	context(context),
	userLogic(settings.userLogic),
	closeRequested(false)
{
}

Window::~Window()
{
}

WindowUserLogic* Window::getUserLogic()
{
	return userLogic;
}

void Window::setUserLogic(WindowUserLogic* v)
{
	if (userLogic) delete userLogic;
	userLogic = v;
}

bool Window::isFocused() const
{
	return system->isFocused(this);
}

void Window::draw() const
{
	if (userLogic)
	{
		userLogic->render({ Vector2f(0,0), (Vector2f)getSize() });
	}
}

void Window::requestClose()
{
	closeRequested = true;
}
