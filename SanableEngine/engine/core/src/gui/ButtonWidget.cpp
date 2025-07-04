#include "gui/ButtonWidget.hpp"

#include "gui/HUD.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/LabelWidget.hpp"

ButtonWidget::ButtonWidget(HUD* hud, ImageWidget* background, SpriteSet bgSprites) :
	ButtonWidgetBase(hud),
	bgSprites(bgSprites)
{
	this->background = background;

	if (background)
	{
		background->getTransform()->setParent(this->getTransform());
		background->getTransform()->setRelativeRenderDepth(-2);
		static_cast<AnchoredPositioning*>(background->getTransform()->getPositioningStrategy())->fillParent();
	}
}

ButtonWidget::ButtonWidget(HUD* hud, ImageWidget* background, SpriteSet bgSprites, Widget* content) :
	ButtonWidget(hud, background, bgSprites)
{
	content->getTransform()->setParent(getContentArea());
}

ButtonWidget::~ButtonWidget()
{
}

void ButtonWidget::setCallback(const std::function<void()>& callback)
{
	this->callback = callback;
}

void ButtonWidget::setSprites(SpriteSet newSprites)
{
	bgSprites = newSprites;
}

void ButtonWidget::setState(ButtonState newState)
{
	ButtonWidgetBase::setState(newState);

	switch (getState())
	{
	#define _X(val) case ButtonState::val: background->setSprite(bgSprites.val); break;
	FOREACH_BUTTON_STATE()
	#undef _X
	}
}

void ButtonWidget::activate() const
{
	if (callback) callback();
}
