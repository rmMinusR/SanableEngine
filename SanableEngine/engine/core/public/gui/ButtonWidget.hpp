#pragma once

#include <string>
#include <functional>

#include "Widget.hpp"

class ImageWidget;
class UISprite;


enum class UIState : uint8_t
{
	#define FOREACH_UISTATE() \
		_X(Normal) \
		_X(Pressed) \
		_X(Disabled)

	#define _X(val) val,
	FOREACH_UISTATE()
	#undef _X
};


class ButtonWidget : public Widget
{
public:

	struct SpriteSet
	{
		#define _X(val) const UISprite* val;
		FOREACH_UISTATE()
		#undef _X
	};

private:
	ImageWidget* background;
	Widget* label;
	SpriteSet sprites;
	UIState state;
	
	std::function<void()> callback;

public:
	ENGINEGUI_API ButtonWidget(HUD* hud, ImageWidget* background, Widget* label, SpriteSet sprites);
	ENGINEGUI_API virtual ~ButtonWidget();
	
	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;

	ENGINEGUI_API void setCallback(const std::function<void()>& callback);
	ENGINEGUI_API virtual void onMouseDown(Vector2f pos) override;
	ENGINEGUI_API virtual void onMouseUp(Vector2f pos) override;
	ENGINEGUI_API virtual void onMouseExit(Vector2f pos) override;
	ENGINEGUI_API virtual void onMouseEnter(Vector2f pos) override;
	ENGINEGUI_API virtual void onDragFinished(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f dragEndPos, Widget* dragEndWidget) override;
	ENGINEGUI_API virtual void onClicked(Vector2f pos) override;

	ENGINEGUI_API void setState(UIState newState);
	ENGINEGUI_API UIState getState() const;
};
