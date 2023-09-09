#include "GUILiveText.hpp"

GUI::LiveText::LiveText(const ControlID& theID, const Vector2D& relativePosition, const Vector2D& size, const Color& color, Font* font, const Font::Alignment& alignment, builder_t&& builder) :
	Text(theID, relativePosition, size, "[LiveText]", color, font, alignment),
	builder(builder)
{
}

void GUI::LiveText::tick(System* system)
{
	updateContent();
}

void GUI::LiveText::updateContent()
{
	std::ostringstream sout;
	builder(sout);
	setText(sout.str());
}
