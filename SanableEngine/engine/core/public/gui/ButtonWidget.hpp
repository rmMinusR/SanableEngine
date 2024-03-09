#pragma once

#include <string>
#include <functional>

#include "Widget.hpp"

class Texture;
class ImageWidget;

class ButtonWidget : public Widget
{
	ImageWidget* background;
	Widget* label;
	
public:
	ENGINEGUI_API ButtonWidget(HUD* hud, ImageWidget* background, Widget* label);
	ENGINEGUI_API virtual ~ButtonWidget();
	
	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;

	//ENGINEGUI_API bool isHovered() const;
	//ENGINEGUI_API bool wasClicked() const;
};
