#pragma once

#include <string>
#include <functional>

#include "Widget.hpp"


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


class ButtonWidgetBase : public Widget
{
private:
	UIState state;
	WidgetTransform* contentTransform;

public:
	ENGINEGUI_API ButtonWidgetBase(HUD* hud);
	ENGINEGUI_API virtual ~ButtonWidgetBase();

	virtual void activate() const = 0;

	ENGINEGUI_API virtual void setState(UIState newState);
	ENGINEGUI_API UIState getState() const;

	ENGINEGUI_API virtual void onMouseDown(Vector2f pos) override;
	ENGINEGUI_API virtual void onMouseUp(Vector2f pos) override;
	ENGINEGUI_API virtual void onMouseExit(Vector2f pos) override;
	ENGINEGUI_API virtual void onMouseEnter(Vector2f pos) override;
	ENGINEGUI_API virtual void onDragFinished(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f dragEndPos, Widget* dragEndWidget) override;
	ENGINEGUI_API virtual void onClicked(Vector2f pos) override;


	ENGINEGUI_API WidgetTransform* getContentArea();
	ENGINEGUI_API const WidgetTransform* getContentArea() const;

	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;
};
