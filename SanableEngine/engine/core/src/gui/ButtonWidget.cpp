#include "gui/ButtonWidget.hpp"

#include "gui/HUD.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/LabelWidget.hpp"

ButtonWidget::ButtonWidget(HUD* hud, ImageWidget* background, Widget* label) :
	Widget(hud)
{
	this->background = background;
	this->label = label;

	background->transform.setParent(&this->transform);
	label     ->transform.setParent(&this->transform);
}
