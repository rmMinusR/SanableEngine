#pragma once

#include <string>
#include "Color.inl"

#include "Widget.hpp"

class Font;

class LabelWidget : public Widget
{
	const Material* material;
	const Font* font;
	std::wstring text;
	Color4<uint8_t> color;
public:
	ENGINEGUI_API LabelWidget(HUD* hud, const Material* material, const Font* font);
	ENGINEGUI_API LabelWidget(HUD* hud, const Material* material, const Font* font, const Color4<uint8_t>& color);
	ENGINEGUI_API virtual ~LabelWidget();

	UIAnchor align;

	ENGINEGUI_API void setText(const std::wstring& newText);
	ENGINEGUI_API void setText(const std::string& newText);

	ENGINEGUI_API virtual void loadModelTransform(Renderer* renderer) const override;
	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;
};
