#pragma once

#include "gui/TabView.hpp"
#include "dllapi.h"

class DraggableTabView : public TabView
{
public:
	EDITORSHARED_API DraggableTabView(HUD* hud, Vector2f tabBtnSize, const Material* tabBtnBgMat, const RadioButtonWidget::SpriteSet tabSprites, TabsLocation tabsLocation);
	EDITORSHARED_API virtual ~DraggableTabView();

protected:
	EDITORSHARED_API virtual RadioButtonWidget* createTabButton(Widget* contentWidget) override; // Actually a DraggableTabButton under the hood
};
