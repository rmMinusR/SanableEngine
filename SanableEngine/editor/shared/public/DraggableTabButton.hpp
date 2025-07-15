#pragma once

#include <functional>

#include "gui/RadioButtonWidget.hpp"
#include "dllapi.h"

class TabView;
class ImageWidget;
class AnchoredPositioning;

class DraggableTabButton : public RadioButtonWidget
{
	std::function<bool(TabView*)> destinationFilter;
	Widget* content;
	ImageWidget* dragIndicator = nullptr;
	AnchoredPositioning* dragPositioner = nullptr;
public:
	EDITORSHARED_API DraggableTabButton(HUD* hud, Widget* content, RadioButtonGroup* group, SpriteSet sprites, const Material* material);
	EDITORSHARED_API ~DraggableTabButton();

	EDITORSHARED_API void setDestinationFilter(std::function<bool(TabView*)> fn);
	EDITORSHARED_API void moveTo(TabView* destination, size_t index); // Ignores filter

	EDITORSHARED_API virtual void onDragStarted(Vector2f dragStartPos, Vector2f currentMousePos) override;
	EDITORSHARED_API virtual void whileDragged(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f currentMousePos, Widget* currentlyHoveredWidget) override;
	EDITORSHARED_API virtual void onDragFinished(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f dragEndPos, Widget* dragEndWidget) override;

private:
	TabView* findTabView(Vector2f pos) const;
};
