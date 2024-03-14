#include "gui/LabelWidget.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Renderer.hpp"
#include "Font.hpp"

LabelWidget::LabelWidget(HUD* hud, Material* material, Font* font) :
	Widget(hud),
	material(material),
	font(font),
	align(Vector2f(0, 0.5f))
{
}

LabelWidget::~LabelWidget()
{
}

void LabelWidget::setText(const std::wstring& newText)
{
	this->text = newText;
}

void LabelWidget::loadModelTransform(Renderer* renderer) const
{
	Vector2f size = font->getRenderedSize(renderer, text);
	renderer->loadTransform(
		glm::translate<float, glm::packed_highp>(
			(glm::mat4)transform,
			Vector3f(align.calcAnchor(transform.getLocalRect().size) + align.calcPivot(size), 0)
		)
	);
}

const Material* LabelWidget::getMaterial() const
{
	return material;
}

void LabelWidget::renderImmediate(Renderer* renderer)
{
	if (material) renderer->drawText(*font, *material, text);
	else
	{
		auto depth = transform.getRenderDepth();
		renderer->drawTextNonShadered(*font, text, Vector3f(0, 0, depth));
	}
}
