#pragma once

#include <string>
#include <SDL_pixels.h>

#include "Widget.hpp"

class Font;

class LabelWidget : public Widget
{
	Material* material;
	Font* font;
	std::wstring text;
	SDL_Color color;
public:
	ENGINEGUI_API LabelWidget(HUD* hud, Material* material, Font* font);
	ENGINEGUI_API LabelWidget(HUD* hud, Material* material, Font* font, const SDL_Color& color);
	ENGINEGUI_API virtual ~LabelWidget();

	UIAnchor align;

	ENGINEGUI_API void setText(const std::wstring& newText);

	ENGINEGUI_API virtual void loadModelTransform(Renderer* renderer) const override;
	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;
};
