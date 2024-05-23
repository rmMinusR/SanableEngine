#pragma once

#include "Widget.hpp"

class GTexture;

class ImageWidget : public Widget
{
	Material* material;
	GTexture* texture;
public:
	ENGINEGUI_API ImageWidget(HUD* hud, Material* material, GTexture* texture);
	ENGINEGUI_API virtual ~ImageWidget();

	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;
};
