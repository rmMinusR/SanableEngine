#pragma once

#include <string>

#include "Widget.hpp"

class Font;

class LabelWidget : public Widget
{
	Material* material;
	Font* font;
	std::wstring text;
public:
	ENGINEGUI_API LabelWidget(HUD* hud, Material* material, Font* font);
	ENGINEGUI_API virtual ~LabelWidget();

	ENGINEGUI_API void setText(const std::wstring& newText);

	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;
};
