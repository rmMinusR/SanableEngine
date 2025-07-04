#include "gui/RadioButtonGroup.hpp"

#include "gui/HUD.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/RadioButtonWidget.hpp"
#include "gui/HorizontalGroupWidget.hpp"
#include "gui/VerticalGroupWidget.hpp"

RadioButtonGroup::RadioButtonGroup(HUD* hud, const Material* btnMat, RadioButtonWidget::SpriteSet sprites, LayoutUtil::LinearElementView btnSizeSettings, bool vertical) :
	Widget(hud),
	sprites(sprites),
	btnMat(btnMat),
	btnSizeSettings(btnSizeSettings)
{
	if(vertical) contentArea = hud->addWidget<VerticalGroupWidget>();
	else contentArea = hud->addWidget<HorizontalGroupWidget>();
	contentArea->getTransform()->setParent(this->getTransform());
	contentArea->getTransform()->setPositioningStrategy<AnchoredPositioning>()->fillParent();
}

RadioButtonGroup::~RadioButtonGroup()
{
}

const Material* RadioButtonGroup::getMaterial() const
{
	return nullptr;
}

void RadioButtonGroup::renderImmediate(Renderer* renderer)
{
}

RadioButtonWidget* RadioButtonGroup::addItem()
{
	RadioButtonWidget* btn = hud->addWidget<RadioButtonWidget>(this, sprites, btnMat);
	btn->updateSprite();

	btn->getTransform()->setParent(contentArea->getTransform());
	AutoLayoutPositioning* positioner = btn->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(contentArea);
	positioner->config = btnSizeSettings;

	return btn;
}

void RadioButtonGroup::select(size_t newSelectionIndex)
{
	RadioButtonWidget* prevSelection = static_cast<RadioButtonWidget*>(contentArea->getTransform()->getChild(curSelection)->getWidget());
	RadioButtonWidget* newSelection = static_cast<RadioButtonWidget*>(contentArea->getTransform()->getChild(newSelectionIndex)->getWidget());

	size_t prevSelectionIdx = curSelection;
	curSelection = newSelectionIndex;

	prevSelection->updateSprite();
	newSelection->updateSprite();

	if(callback) callback(prevSelectionIdx, curSelection);
}

size_t RadioButtonGroup::getSelectionIndex() const
{
	return curSelection;
}

void RadioButtonGroup::setCallback(std::function<void(size_t, size_t)> callback)
{
	this->callback = callback;
}
