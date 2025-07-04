#include "gui/ButtonWidgetBase.hpp"

#include "gui/HUD.hpp"

ButtonWidgetBase::ButtonWidgetBase(HUD* hud) :
	Widget(hud)
{
	getTransform()->setRelativeRenderDepth(1);

	contentTransform = hud->getMemory()->create<WidgetTransform>(nullptr, hud);
	contentTransform->setRelativeRenderDepth(-1);
}

ButtonWidgetBase::~ButtonWidgetBase()
{
	hud->getMemory()->destroy(contentTransform);
}

const Material* ButtonWidgetBase::getMaterial() const
{
	return nullptr;
}

void ButtonWidgetBase::renderImmediate(Renderer* renderer)
{
}

void ButtonWidgetBase::onMouseDown(Vector2f pos)
{
	if (state != ButtonState::Disabled)
	{
		setState(ButtonState::Pressed);
	}
}

void ButtonWidgetBase::onMouseUp(Vector2f pos)
{
	if (state != ButtonState::Disabled)
	{
		setState(ButtonState::Normal);
	}
}

void ButtonWidgetBase::onMouseExit(Vector2f pos)
{
	if (state != ButtonState::Disabled)
	{
		setState(ButtonState::Normal);
	}
}

void ButtonWidgetBase::onMouseEnter(Vector2f pos)
{
	if (state != ButtonState::Disabled)
	{
		setState(ButtonState::Normal); //TODO set to Pressed instead if a mouse button is down
	}
}

void ButtonWidgetBase::onDragFinished(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f dragEndPos, Widget* dragEndWidget)
{
	if (dragStartWidget == this && dragEndWidget == this)
	{
		activate();
	}
}

void ButtonWidgetBase::onClicked(Vector2f pos)
{
	if (state != ButtonState::Disabled) activate();
}

void ButtonWidgetBase::setState(ButtonState newState)
{
	state = newState;
}

ButtonState ButtonWidgetBase::getState() const
{
	return state;
}

WidgetTransform* ButtonWidgetBase::getContentArea()
{
	return contentTransform;
}

const WidgetTransform* ButtonWidgetBase::getContentArea() const
{
	return contentTransform;
}
