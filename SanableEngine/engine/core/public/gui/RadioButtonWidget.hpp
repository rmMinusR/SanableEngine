#pragma once

#include "ButtonWidgetBase.hpp"

class ImageWidget;
class UISprite;
class RadioButtonGroup;

class RadioButtonWidget : public ButtonWidgetBase
{
public:
	struct SpriteSet
	{
		const UISprite* normal;
		const UISprite* pressed;
		const UISprite* selected;
	};

	ENGINEGUI_API RadioButtonWidget(HUD* hud, RadioButtonGroup* group, SpriteSet sprites, const Material* material);
	ENGINEGUI_API virtual ~RadioButtonWidget();

	ENGINEGUI_API ImageWidget* getBackground();
	ENGINEGUI_API const ImageWidget* getBackground() const;

	ENGINEGUI_API bool isSelected() const;

	ENGINEGUI_API virtual void activate() const final;
	
protected:
	friend class RadioButtonGroup;
	ENGINEGUI_API void updateSprite();

protected:
	RadioButtonGroup* group;
	ImageWidget* background;
	SpriteSet sprites;

	ENGINEGUI_API virtual void setState(ButtonState newState) override final;
};
