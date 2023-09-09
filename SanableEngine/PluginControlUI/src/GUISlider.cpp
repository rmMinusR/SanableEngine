#include "GUISlider.hpp"

#include <optional>
#include <algorithm>

#include "InterfaceEvents.h"
#include "InputSystem.h"
#include "GraphicsSystem.h"

GUI::Slider::Slider(ControlID theID, const Vector2D& relativePosition, const Vector2D& size, float fillValue, float lowerBound, float upperBound, const ControlColorProfile& knobColorProfile, const Color& channelColor, const update_callback_t& onUpdate) :
	Slider(theID, relativePosition, size, fillValue, lowerBound, upperBound, 1.0f, knobColorProfile, channelColor, onUpdate)
{
}

GUI::Slider::Slider(ControlID theID, const Vector2D& relativePosition, const Vector2D& size, float fillValue, float lowerBound, float upperBound, float mBase, const ControlColorProfile& knobColorProfile, const Color& channelColor, const update_callback_t& onUpdate) :
	Control(theID, relativePosition, size, knobColorProfile),
	channelColor(channelColor),
	onUpdate(onUpdate),
	lowerBound(lowerBound),
	upperBound(upperBound),
	mBase(mBase)
{
	calcLogBounds();
	setFillValue(fillValue); //Sends initial event, ensures non-stale state
}

void GUI::Slider::draw(GraphicsBuffer* dest)
{
	if (isActive())
	{
		float channelWidth = size.getX() - knobSize; //Pad by knobSize/2 on either side

		//Draw channel
		GraphicsSystem::renderRect(*dest, cachedRenderPosition+Vector2D(knobSize/2, size.getY()/2.0f-channelSize/2.0f), (int)channelWidth, (int)channelSize, channelColor, true);

		//Draw knob
		GraphicsSystem::renderRect(*dest, cachedRenderPosition+Vector2D(channelWidth*fillRatio, 0.0f), (int)knobSize, (int)size.getY(), getCurrentColor(), true);
	}
}

void GUI::Slider::handleEvent(const Event& theEvent)
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
				else if (getCurrentState() == ControlState::PRESSED && mouseButton->getPressedState() == InputSystem::PressState::RELEASED) setCurrentState(ControlState::NORMAL);
			}
		}

		//Hover/unhover
		if(mousePos.has_value())
		{
				 if (getCurrentState() == ControlState::NORMAL  &&  intersects(mousePos.value())) setCurrentState(ControlState::HOVERED);
			else if (getCurrentState() == ControlState::HOVERED && !intersects(mousePos.value())) setCurrentState(ControlState::NORMAL);
		}

		//Update fillRatio
		if (getCurrentState() == ControlState::PRESSED && mousePos.has_value())
		{
			//Inverse-lerp to find new fill amount
			setFillRatio( (mousePos.value().getX()-cachedAbsolutePosition.getX()) / size.getX() );
		}

	}
}

const float& GUI::Slider::getFillValue() const
{
	return lowerBound;
}

void GUI::Slider::setFillValue(const float& val, bool sendEvent, bool doClamp)
{
	fillValue = val;
	if (doClamp) fillValue = std::clamp<float>(fillValue, lowerBound, upperBound);
	float effectiveFillVal = isLogarithmic() ? log(fillValue)/log(mBase) : fillValue;

	//Inverse-lerp to find new fill ratio
	//effectiveFillVal = (1-fillRatio)*logLowerBound + fillRatio*logUpperBound
	//effectiveFillVal = logLowerBound - fillRatio*logLowerBound + fillRatio*logUpperBound
	//effectiveFillVal = logLowerBound + fillRatio*(logUpperBound - logLowerBound)
	//effectiveFillVal = logLowerBound + fillRatio*(logUpperBound - logLowerBound)
	//effectiveFillVal - logLowerBound = fillRatio*(logUpperBound - logLowerBound)
	//(effectiveFillVal - logLowerBound)/(logUpperBound - logLowerBound) = fillRatio
	fillRatio = (effectiveFillVal - logLowerBound)/(logUpperBound - logLowerBound);

	//Run callback
	if(sendEvent) onUpdate(fillValue);
}

const float& GUI::Slider::getFillRatio() const
{
	return fillRatio;
}

void GUI::Slider::setFillRatio(const float& amt, bool sendEvent, bool doClamp)
{
	//Clamp to 0-1
	fillRatio = amt;
	if (doClamp) fillRatio = std::clamp<float>(fillRatio, 0, 1);
	calcFillValue(); //lerp

	//Run callback
	if(sendEvent) onUpdate(fillValue);
}

void GUI::Slider::calcLogBounds()
{
	if (isLogarithmic())
	{
		float base = log(mBase);
		logLowerBound = log(lowerBound) / base;
		logUpperBound = log(upperBound) / base;
	}
	else
	{
		logLowerBound = lowerBound;
		logUpperBound = upperBound;
	}
}

bool GUI::Slider::isLogarithmic() const
{
	return mBase != 1;
}

void GUI::Slider::calcFillValue()
{
	//lerp
	if(isLogarithmic()) fillValue = pow(mBase, (1-fillRatio)*logLowerBound + fillRatio*logUpperBound);
	else fillValue = (1-fillRatio)*lowerBound + fillRatio*upperBound;
}
