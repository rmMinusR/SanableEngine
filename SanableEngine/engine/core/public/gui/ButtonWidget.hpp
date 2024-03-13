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
	
	std::function<void()> callback;

public:
	ENGINEGUI_API ButtonWidget(HUD* hud, ImageWidget* background, Widget* label);
	ENGINEGUI_API virtual ~ButtonWidget();
	
	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;

	ENGINEGUI_API void setCallback(const std::function<void()>& callback);
	ENGINEGUI_API virtual bool onMouseDown(Vector2f pos) override;
};
