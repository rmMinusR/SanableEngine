#pragma once

#include "Widget.hpp"

class UISprite;
class ImageWidget;

class GroupResizeHandle : public Widget
{
	ImageWidget* background;
	WidgetSocket socket;

	const UISprite* spriteNormal;
	const UISprite* spriteDragged;

public:
	ENGINEGUI_API GroupResizeHandle(HUD* hud, ImageWidget* background, const UISprite* spriteNormal, const UISprite* spriteDragged);
	ENGINEGUI_API virtual ~GroupResizeHandle();

	ENGINEGUI_API WidgetSocket* getSocket();
	ENGINEGUI_API const WidgetSocket* getSocket() const;

	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;

	ENGINEGUI_API virtual void onDragStarted(Vector2f dragStartPos, Vector2f currentMousePos) override;
	ENGINEGUI_API virtual void whileDragged(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f currentMousePos, Widget* currentlyHoveredWidget) override;
	ENGINEGUI_API virtual void onDragFinished(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f dragEndPos, Widget* dragEndWidget) override;
};
