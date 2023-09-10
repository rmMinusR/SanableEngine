#pragma once

#include "Color.h"
#include "Font.h"

#include "GUIElement.hpp"

namespace GUI
{

	//Text displays a string. The alignment's a bit funky, but that's because I was planning to replace it with a more-robust system, which I didn't get to.

	class Text : public Element
	{
	public:
		Text(const ControlID& theID, const Vector3<float>& relativePosition, const Vector3<float>& size, const std::string& text, const Color& color, Font* font, const Font::Alignment& alignment);

		virtual void draw(GraphicsBuffer* dest) override;

		void setText(std::string&& text);

	protected:
		std::string text;
		Color color;
		Font* font;
		Font::Alignment alignment;
	};

}