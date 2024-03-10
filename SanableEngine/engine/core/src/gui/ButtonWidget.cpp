#include "gui/ButtonWidget.hpp"

#include "gui/HUD.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/LabelWidget.hpp"

ButtonWidget::ButtonWidget(HUD* hud, ImageWidget* background, Widget* label) :
	Widget(hud)
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
