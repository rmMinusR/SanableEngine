#include "GUICheckbox.hpp"

#include <optional>

#include "InterfaceEvents.h"
#include "GraphicsSystem.h"

GUI::Checkbox::Checkbox(const ControlID& theID, const Vector2D& relativePosition, const Vector2D& size, const bool& isChecked, const float& checkSizeRatio, const ControlColorProfile& colorProfile, const Color& checkColor, callback_func_t&& callback) :
	Control(theID, relativePosition, size, colorProfile),
	isChecked(isChecked),
	checkSizeRatio(checkSizeRatio),
	checkColor(checkColor),
	callback(callback)
{
	callback(isChecked);
}

void GUI::Checkbox::draw(GraphicsBuffer* dest)
{
	GraphicsSystem::renderRect(*dest, cachedRenderPosition, (int)size.getX(), (int)size.getY(), getCurrentColor(), true);

	if (isChecked)
	{
		Vector2D checkSize = size*checkSizeRatio;
		Vector2D offset = size*(1-checkSizeRatio)/2;
		GraphicsSystem::renderRect(*dest, cachedRenderPosition+offset, (int)checkSize.getX(), (int)checkSize.getY(), checkColor, true);
	}
}

void GUI::Checkbox::handleEvent(const Event& theEvent)
{
	if(isActive())
	{

		std::optional<Vector2D> mousePos = std::nullopt;

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
					 if (getCurrentState() == ControlState::HOVERED && mouseButton->getPressedState() == InputSystem::PressState::PRESSED ) setCurrentState(ControlState::PRESSED);
				else if (getCurrentState() == ControlState::PRESSED && mouseButton->getPressedState() == InputSystem::PressState::RELEASED)
				{
					setCurrentState(ControlState::NORMAL);
					isChecked = !isChecked;
					callback(isChecked);
				}
			}
		}

		//Hover/unhover
		if(mousePos.has_value())
		{
				 if (getCurrentState() == ControlState::NORMAL  &&  intersects(mousePos.value())) setCurrentState(ControlState::HOVERED);
			else if (getCurrentState() == ControlState::HOVERED && !intersects(mousePos.value())) setCurrentState(ControlState::NORMAL);
		}
	}
}
