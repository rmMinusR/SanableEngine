#include "gui/ButtonWidget.hpp"

#include "gui/HUD.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/LabelWidget.hpp"

ButtonWidget::ButtonWidget(HUD* hud, ImageWidget* background, SpriteSet bgSprites) :
	Widget(hud),
	contentSocket(hud, this),
	bgSprites(bgSprites)
{
	this->background = background;

	getTransform()->setRelativeRenderDepth(1);
	
	if (background)
	{
		background->getTransform()->setParent(this->getTransform());
		background->getTransform()->setRelativeRenderDepth(-2);
		static_cast<AnchoredPositioning*>(background->getTransform()->getPositioningStrategy())->fillParent();
	}

	contentSocket.setRelativeRenderDepth(-1);
}

ButtonWidget::ButtonWidget(HUD* hud, ImageWidget* background, SpriteSet bgSprites, Widget* content) :
	ButtonWidget(hud, background, bgSprites)
{
	contentSocket.put(content);
}

ButtonWidget::~ButtonWidget()
{
}

const Material* ButtonWidget::getMaterial() const
{
	return nullptr;
}

void ButtonWidget::renderImmediate(Renderer* renderer)
{
}

void ButtonWidget::setCallback(const std::function<void()>& callback)
{
	this->callback = callback;
}

void ButtonWidget::onMouseDown(Vector2f pos)
{
	if (state != UIState::Disabled)
	{
		setState(UIState::Pressed);
	}
}

void ButtonWidget::onMouseUp(Vector2f pos)
{
	if (state != UIState::Disabled)
	{
		setState(UIState::Normal);
	}
}

void ButtonWidget::onMouseExit(Vector2f pos)
{
	if (state != UIState::Disabled)
	{
		setState(UIState::Normal);
	}
}

void ButtonWidget::onMouseEnter(Vector2f pos)
{
	if (state != UIState::Disabled)
	{
		setState(UIState::Normal); //TODO set to Pressed instead if a mouse button is down
	}
}

void ButtonWidget::onDragFinished(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f dragEndPos, Widget* dragEndWidget)
{
	if (dragStartWidget == this && dragEndWidget == this)
	{
		if (callback) callback();
	}
}

void ButtonWidget::onClicked(Vector2f pos)
{
	if (state != UIState::Disabled && callback) callback();
}

void ButtonWidget::setState(UIState newState)
{
	state = newState;

	switch (state)
	{
	#define _X(val) case UIState::val: background->setSprite(bgSprites.val); break;
	FOREACH_UISTATE()
	#undef _X
	}
}

UIState ButtonWidget::getState() const
{
	return state;
}

WidgetSocket* ButtonWidget::getContentSocket()
{
	return &contentSocket;
}

const WidgetSocket* ButtonWidget::getContentSocket() const
{
	return &contentSocket;
}
