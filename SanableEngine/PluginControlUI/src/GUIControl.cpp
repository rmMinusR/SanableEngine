#include "GUIControl.hpp"

#include "EventSystem.h"

GUI::Control::Control(const ControlID& theID, const Vector3<float>& relativePosition, const Vector3<float>& size, const ControlColorProfile& colorProfile) :
	Element(theID, relativePosition, size),
	colorProfile(colorProfile),
	currentState(ControlState::NORMAL)
{
	EventSystem* pEventSystem = EventSystem::getInstance();
	pEventSystem->addListener(Event::MOUSE_MOVE_EVENT, this);
	pEventSystem->addListener(Event::MOUSE_BUTTON_EVENT, this);
	pEventSystem->addListener(Event::KEYBOARD_EVENT, this);
}

GUI::Control::~Control()
{
	EventSystem::getInstance()->removeListenerFromAllEvents(this);
}
