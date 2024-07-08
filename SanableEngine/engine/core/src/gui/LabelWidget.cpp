#include "gui/LabelWidget.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Renderer.hpp"
#include "Font.hpp"

LabelWidget::LabelWidget(HUD* hud, const Material* material, const Font* font) :
	LabelWidget(hud, material, font, SDL_Color{ 255, 255, 255, 255 })
{
}

LabelWidget::LabelWidget(HUD* hud, const Material* material, const Font* font, const SDL_Color& color) :
	Widget(hud),
	material(material),
	font(font),
	align(Vector2f(0, 0.5f)),
	color(color)
{
}

LabelWidget::~LabelWidget()
{
}

void LabelWidget::setText(const std::wstring& newText)
{
	this->text = newText;
}

void LabelWidget::setText(const std::string& newText)
{
	this->text.resize(newText.length(), 0);
	std::mbstowcs(text.data(), newText.data(), newText.length());
}

void LabelWidget::loadModelTransform(Renderer* renderer) const
{
	Vector2f size = font->getRenderedSize(renderer, text);
	renderer->loadTransform(
		glm::translate<float, glm::packed_highp>(
			(glm::mat4)*getTransform(),
			Vector3f(align.calcAnchor(getTransform()->getLocalRect().size) + align.calcPivot(size), 0)
		)
	);
}

const Material* LabelWidget::getMaterial() const
{
	return material;
}

void LabelWidget::renderImmediate(Renderer* renderer)
{
	if (material) renderer->drawText(*font, *material, text, color);
	else
	{
		auto depth = getTransform()->getRenderDepth();
		renderer->drawTextNonShadered(*font, text, Vector3f(0, 0, depth));
	}
}
