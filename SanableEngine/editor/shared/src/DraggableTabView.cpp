#include "DraggableTabView.hpp"

#include "gui/HUD.hpp"
#include "DraggableTabButton.hpp"

DraggableTabView::DraggableTabView(HUD* hud, Vector2f tabBtnSize, const Material* tabBtnBgMat, const RadioButtonWidget::SpriteSet tabSprites, TabsLocation tabsLocation) :
	TabView(hud, tabBtnSize, tabBtnBgMat, tabSprites, tabsLocation)
{
}

DraggableTabView::~DraggableTabView()
{
}

RadioButtonWidget* DraggableTabView::createTabButton(Widget* contentWidget)
{
	DraggableTabButton* tabBtn = hud->addWidget<DraggableTabButton>(contentWidget, tabArea, tabArea->getSprites(), tabArea->getButtonMaterial());
	tabArea->addItem(tabBtn);
	return tabBtn;
}
