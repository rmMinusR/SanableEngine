#pragma once

#include "gui/Widget.hpp"
#include "dllapi.h"

class UISprite;
class ImageWidget;

class GroupResizeHandle : public Widget
{
	ImageWidget* background;
	WidgetSocket socket;

	const UISprite* spriteNormal;
	const UISprite* spriteDragged;

public:
	EDITORSHARED_API GroupResizeHandle(HUD* hud, ImageWidget* background, const UISprite* spriteNormal, const UISprite* spriteDragged);
	EDITORSHARED_API virtual ~GroupResizeHandle();

	EDITORSHARED_API WidgetSocket* getSocket();
	EDITORSHARED_API const WidgetSocket* getSocket() const;

	EDITORSHARED_API virtual const Material* getMaterial() const override;
	EDITORSHARED_API virtual void renderImmediate(Renderer* renderer) override;

	EDITORSHARED_API virtual void onDragStarted(Vector2f dragStartPos, Vector2f currentMousePos) override;
	EDITORSHARED_API virtual void whileDragged(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f currentMousePos, Widget* currentlyHoveredWidget) override;
	EDITORSHARED_API virtual void onDragFinished(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f dragEndPos, Widget* dragEndWidget) override;
};
