#pragma once

#include "Color.inl"

#include "Widget.hpp"

class UISprite;

class ImageWidget : public Widget
{
	const Material* material;
	const UISprite* sprite;
	Color4<uint8_t> tintColor;
public:
	ENGINEGUI_API ImageWidget(HUD* hud, const Material* material, const UISprite* sprite);
	ENGINEGUI_API virtual ~ImageWidget();

	ENGINEGUI_API void setSprite(const UISprite* newSprite);
	ENGINEGUI_API const UISprite* getSprite() const;
	ENGINEGUI_API void setTintColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	ENGINEGUI_API void setMaterial(const Material* mat);
	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;
};
