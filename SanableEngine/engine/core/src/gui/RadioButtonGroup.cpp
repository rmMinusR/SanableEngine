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

const Material* RadioButtonGroup::getButtonMaterial() const
{
	return btnMat;
}

void RadioButtonGroup::setButtonMaterial(const Material* mat)
{
	btnMat = mat;
	for (size_t i = 0; i < contentArea->getTransform()->getChildrenCount(); ++i)
	{
		RadioButtonWidget* w = static_cast<RadioButtonWidget*>(contentArea->getTransform()->getChild(i)->getWidget());
		w->background->setMaterial(mat);
	}
}

RadioButtonWidget::SpriteSet RadioButtonGroup::getSprites() const
{
	return sprites;
}

void RadioButtonGroup::setSprites(RadioButtonWidget::SpriteSet sprites)
{
	this->sprites = sprites;
	for (size_t i = 0; i < contentArea->getTransform()->getChildrenCount(); ++i)
	{
		RadioButtonWidget* w = static_cast<RadioButtonWidget*>(contentArea->getTransform()->getChild(i)->getWidget());
		w->sprites = sprites;
		w->updateSprite();
	}
}

RadioButtonWidget* RadioButtonGroup::addItem()
{
	return insertItem(-1);
}

void RadioButtonGroup::addItem(RadioButtonWidget* btn)
{
	insertItem(btn, -1);
}

RadioButtonWidget* RadioButtonGroup::insertItem(size_t index)
{
	RadioButtonWidget* btn = hud->addWidget<RadioButtonWidget>(this, sprites, btnMat);
	insertItem(btn, index);
	return btn;
}

void RadioButtonGroup::insertItem(RadioButtonWidget* btn, size_t index)
{
	if (index == -1)
	{
		index = contentArea->getTransform()->getChildrenCount();
	}
	else assert(0 <= index && index <= getTransform()->getChildrenCount());

	btn->getTransform()->setParent(contentArea->getTransform());
	btn->getTransform()->setChildIndex(index);
	AutoLayoutPositioning* positioner = btn->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(contentArea);
	positioner->config = btnSizeSettings;

	if (curSelection >= index && contentArea->getTransform()->getChildrenCount() > 1)
	{
		curSelection++; // Preserve focused item
	}

	btn->updateSprite();
}

void RadioButtonGroup::detachItem(RadioButtonWidget* btn)
{
	assert(btn->getTransform()->getParent() == contentArea->getTransform());

	if (curSelection > btn->getTransform()->getChildIndex() && curSelection > 0)
	{
		curSelection--; // Preserve focused item
	}

	btn->getTransform()->setParent(hud->getRootTransform());
}

void RadioButtonGroup::removeItem(RadioButtonWidget* btn)
{
	assert(btn->getTransform()->getParent() == contentArea->getTransform());

	if (curSelection > btn->getTransform()->getChildIndex() && curSelection > 0)
	{
		curSelection--; // Preserve focused item
	}

	hud->destroyWidget(btn);
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
