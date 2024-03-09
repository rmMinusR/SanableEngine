#include "gui/LabelWidget.hpp"

#include "Renderer.hpp"

LabelWidget::LabelWidget(Material* material, Font* font) :
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

	constexpr SDL_Color col{ 1, 1, 1, 1 };
	renderer->drawText(*font, col, text, Vector3f(r.x, r.y, depth));
}
