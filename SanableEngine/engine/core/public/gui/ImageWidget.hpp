#pragma once

#include "Widget.hpp"

class UISprite;

class ImageWidget : public Widget
{
	Material* material;
	const UISprite* sprite;
public:
	ENGINEGUI_API ImageWidget(HUD* hud, Material* material, const UISprite* sprite);
	ENGINEGUI_API virtual ~ImageWidget();

	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;
};
