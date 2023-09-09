#include "GUIText.hpp"

#include <GraphicsSystem.h>

GUI::Text::Text(const ControlID& theID, const Vector2D& relativePosition, const Vector2D& size, const std::string& text, const Color& color, Font* font, const Font::Alignment& alignment) :
	Element(theID, relativePosition, size),
	text(text),
	color(color),
	font(font),
	alignment(alignment)
{

}

void GUI::Text::draw(GraphicsBuffer* dest)
{
	if (isActive())
	{
		Vector2D anchorPos;
		switch (alignment)
		{
		case Font::Alignment::LEFT:   anchorPos = cachedRenderPosition             ; break;
		case Font::Alignment::CENTER: anchorPos = cachedRenderPosition + size * 0.5; break;
		case Font::Alignment::RIGHT:  anchorPos = cachedRenderPosition + size      ; break;
		}

		//Compensate for text height
		anchorPos -= Vector2D(0, font->getSize()/2);

		GraphicsSystem::writeText(*dest, anchorPos, *font, color, text, alignment);
	}
}

void GUI::Text::setText(std::string&& text)
{
	this->text = text;
}
