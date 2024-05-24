#include "gui/ButtonWidget.hpp"

#include "gui/HUD.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/LabelWidget.hpp"

ButtonWidget::ButtonWidget(HUD* hud, ImageWidget* background, Widget* label, SpriteSet sprites) :
	Widget(hud),
	sprites(sprites)
{
	this->background = background;
	this->label = label;

	if (background)
	{
		background->transform.setParent(&this->transform);
		background->transform.setRenderDepth(-1);
		background->transform.fillParent();
	}

	if (label)
	{
		label->transform.setParent(&this->transform);
		label->transform.setRenderDepth(0);
		label->transform.fillParent();
	}
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
		if (callback) callback();
	}
}

void ButtonWidget::setState(UIState newState)
{
	state = newState;

	switch (state)
	{
	#define _X(val) case UIState::val: background->setSprite(sprites.val); break;
	FOREACH_UISTATE()
	#undef _X
	}
}

UIState ButtonWidget::getState() const
{
	return state;
}
