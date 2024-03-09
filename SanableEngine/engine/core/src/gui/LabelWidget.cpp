#include "gui/LabelWidget.hpp"

#include "Renderer.hpp"

LabelWidget::LabelWidget(HUD* hud, Material* material, Font* font) :
	Widget(hud),
	material(material),
	font(font)
{
}

LabelWidget::~LabelWidget()
{
}

void LabelWidget::setText(const std::wstring& newText)
{
	this->text = newText;
}

const Material* LabelWidget::getMaterial() const
{
	return material;
}

void LabelWidget::renderImmediate(Renderer* renderer)
{
	Rect<float> r = transform.getRect();
	auto depth = transform.getRenderDepth();

	if (material) renderer->drawText(*font, *material, text);
	else renderer->drawTextNonShadered(*font, text, Vector3f(r.x, r.y, depth));
}
