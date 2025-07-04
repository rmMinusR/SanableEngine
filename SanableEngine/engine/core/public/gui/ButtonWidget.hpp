#pragma once

#include <string>
#include <functional>

#include "ButtonWidgetBase.hpp"

class ImageWidget;
class UISprite;


class ButtonWidget : public ButtonWidgetBase
{
public:

	struct SpriteSet
	{
		#define _X(val) const UISprite* val;
		FOREACH_UISTATE()
		#undef _X
	};

protected:
	ImageWidget* background;
	SpriteSet bgSprites;
	
	std::function<void()> callback;

public:
	ENGINEGUI_API ButtonWidget(HUD* hud, ImageWidget* background, SpriteSet bgSprites);
	ENGINEGUI_API ButtonWidget(HUD* hud, ImageWidget* background, SpriteSet bgSprites, Widget* content);
	ENGINEGUI_API virtual ~ButtonWidget();
	
	ENGINEGUI_API void setCallback(const std::function<void()>& callback);
	ENGINEGUI_API void setSprites(SpriteSet newSprites);

	ENGINEGUI_API virtual void setState(UIState newState) override;
	ENGINEGUI_API virtual void activate() const override;
};
