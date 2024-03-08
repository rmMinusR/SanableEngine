#pragma once

#include "Widget.hpp"

class Texture;

class ImageWidget : public Widget
{
	Material* material;
	Texture* texture;
public:
	ENGINEGUI_API ImageWidget(Material* material, Texture* texture);
	ENGINEGUI_API virtual ~ImageWidget();

	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;
};
