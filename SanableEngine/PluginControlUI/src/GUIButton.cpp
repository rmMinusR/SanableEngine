#include "GUIButton.hpp"

#include <optional>

#include "InterfaceEvents.h"
#include "InputSystem.h"
#include "GraphicsSystem.h"

GUI::Button::Button(const ControlID& theID, const Vector3<float>& relativePosition, const Vector3<float>& size, const ControlColorProfile& colorProfile, GraphicsBufferManager* gbm, callback_t&& onClicked) :
	Control(theID, relativePosition, size, colorProfile),
	onClicked(onClicked)
{
	panel = new Panel(generateID(), relativePosition, size, colorProfile.normal, gbm);
}

GUI::Button::Button(const ControlID& theID, const Vector3<float>& relativePosition, const Vector3<float>& size, const ControlColorProfile& colorProfile, GraphicsBufferManager* gbm, callback_t&& onClicked, std::initializer_list<Element*> labels) :
	Control(theID, relativePosition, size, colorProfile),
	onClicked(onClicked)
{
	panel = new Panel(generateID(), relativePosition, size, colorProfile.normal, gbm, labels);
}

GUI::Button::~Button()
{
	delete panel;
}

void GUI::Button::tick(System* system)
{
	if(isActive()) panel->setBackgroundColor(Control::getCurrentColor());
}

void GUI::Button::draw(GraphicsBuffer* dest)
{
	if (isActive())
	{
		panel->draw(dest);
	}
}

void GUI::Button::rebuildAbsoluteTransform(const Vector3<float>& parentAbsPos)
{
	Element::rebuildAbsoluteTransform(parentAbsPos);
	panel->rebuildAbsoluteTransform(parentAbsPos);
}

void GUI::Button::rebuildRenderTransform(const Vector3<float>& rootRenderPos)
{
	Element::rebuildRenderTransform(rootRenderPos);
	panel->rebuildRenderTransform(rootRenderPos);
}

void GUI::Button::handleEvent(const Event& theEvent)
{
	if(Element::isActive())
	{
		std::optional<Vector3<float>> mousePos = std::nullopt;

		if (theEvent.getType() == Event::EventType::MOUSE_MOVE_EVENT)
		{
			MouseMoveEvent const* mouseMove = dynamic_cast<MouseMoveEvent const*>(&theEvent);
			mousePos = mouseMove->getLocation();
		}
		else if (theEvent.getType() == Event::EventType::MOUSE_BUTTON_EVENT)
		{
			MouseButtonEvent const* mouseButton = dynamic_cast<MouseButtonEvent const*>(&theEvent);
			mousePos = mouseButton->getLocation();
			//Button down/up
			if (mouseButton->getButton() == InputSystem::MouseButton::LEFT)
			{
					 if (getCurrentState() == ControlState::HOVERED && mouseButton->getPressedState() == InputSystem::PressState::PRESSED) setCurrentState(ControlState::PRESSED);
				else if (getCurrentState() == ControlState::PRESSED && mouseButton->getPressedState() == InputSystem::PressState::RELEASED)
				{
					setCurrentState(ControlState::NORMAL);
					onClicked();
				}
			}
		}

		if(mousePos.has_value()) {
			//Hover/unhover
				 if (getCurrentState() == ControlState::NORMAL  &&  Control::intersects(mousePos.value())) setCurrentState(ControlState::HOVERED);
			else if (getCurrentState() == ControlState::HOVERED && !Control::intersects(mousePos.value())) setCurrentState(ControlState::NORMAL);

			panel->markDirty();
		}

	}
}

void GUI::Button::setOnClicked(callback_t&& onClicked)
{
	this->onClicked = onClicked;
}

void GUI::Button::visit(const std::function<void(Element*)>& visitor)
{
	visitor(this);
	panel->visit(visitor);
}

bool GUI::Button::isDirty() const
{
	return Element::isDirty() | panel->isDirty();
}

void GUI::Button::clearDirty()
{
	Element::clearDirty();
	panel->clearDirty();
}
