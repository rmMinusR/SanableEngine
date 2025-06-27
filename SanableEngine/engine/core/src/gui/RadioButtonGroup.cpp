#include "gui/RadioButtonGroup.hpp"

#include "gui/HUD.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/ButtonWidget.hpp"
#include "gui/HorizontalGroupWidget.hpp"
#include "gui/VerticalGroupWidget.hpp"

RadioButtonGroup::RadioButtonGroup(HUD* hud, const Material* btnMat, SpriteSet sprites, LayoutUtil::LinearElementView btnSizeSettings, bool vertical) :
	Widget(hud),
	sprites(sprites),
	btnMat(btnMat),
	btnSizeSettings(btnSizeSettings)
{
	if(vertical) contentArea = hud->addWidget<VerticalGroupWidget>();
	else contentArea = hud->addWidget<HorizontalGroupWidget>();
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

ButtonWidget* RadioButtonGroup::addItem()
{
	ImageWidget* btnBg = hud->addWidget<ImageWidget>(btnMat, sprites.normal);

	ButtonWidget* btn = hud->addWidget<ButtonWidget>(btnBg, ButtonWidget::SpriteSet{ sprites.normal, sprites.pressed, sprites.normal });
	if (contentArea->getTransform()->getChildrenCount() == 0)
	{
		btn->setSprites({ sprites.selected, sprites.normal, sprites.normal });
	}
	btn->getTransform()->setParent(contentArea->getTransform());
	btn->setCallback(
		[btn, this]()
		{
			size_t idx = btn->getTransform()->getChildIndex();
			this->select(idx);
		}
	);

	AutoLayoutPositioning* positioner = btn->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(contentArea);
	positioner->config = btnSizeSettings;

	return btn;
}

void RadioButtonGroup::select(size_t newSelectionIndex)
{
	ButtonWidget* prevSelection = static_cast<ButtonWidget*>(contentArea->getTransform()->getChild(curSelection)->getWidget());
	ButtonWidget* newSelection = static_cast<ButtonWidget*>(contentArea->getTransform()->getChild(newSelectionIndex)->getWidget());
	prevSelection->setSprites({ sprites.normal, sprites.pressed, sprites.normal }); // Restore normal behavior
	newSelection->setSprites({ sprites.selected, sprites.normal, sprites.normal }); // Set selected and disable press visual

	size_t prevSelectionIdx = curSelection;
	curSelection = newSelectionIndex;
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
