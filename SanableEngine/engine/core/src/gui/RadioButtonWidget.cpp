#include "gui/RadioButtonWidget.hpp"

#include "gui/HUD.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/RadioButtonGroup.hpp"

RadioButtonWidget::RadioButtonWidget(HUD* hud, RadioButtonGroup* group, SpriteSet sprites, const Material* material) :
	ButtonWidgetBase(hud),
	group(group),
	sprites(sprites)
{
	background = hud->addWidget<ImageWidget>(material, sprites.normal);
	background->getTransform()->setParent(getTransform());
	background->getTransform()->setPositioningStrategy<AnchoredPositioning>()->fillParent();
}

RadioButtonWidget::~RadioButtonWidget()
{
	hud->destroyWidget(background);
}

bool RadioButtonWidget::isSelected() const
{
	return group->getSelectionIndex() == getTransform()->getChildIndex();
}

void RadioButtonWidget::activate() const
{
	group->select(getTransform()->getChildIndex());
}

void RadioButtonWidget::updateSprite()
{
	if (isSelected()) background->setSprite(sprites.selected);
	else background->setSprite(getState() == ButtonState::Pressed ? sprites.pressed : sprites.normal);
}

void RadioButtonWidget::setState(ButtonState newState)
{
	ButtonWidgetBase::setState(newState);
	updateSprite();
}

ImageWidget* RadioButtonWidget::getBackground()
{
	return background;
}

const ImageWidget* RadioButtonWidget::getBackground() const
{
	return background;
}
