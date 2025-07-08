#pragma once

#include <string_view>

#include "math/Vector2.inl"
#include "RadioButtonGroup.hpp"

class ButtonWidget;
class LinearLayoutGroupWidget;

class TabView : public Widget
{
public:
	enum class TabsLocation
	{
		Top    = 0b00,
		Bottom = 0b01,
		Left   = 0b10,
		Right  = 0b11
	};

	ENGINEGUI_API TabView(HUD* hud, Vector2f tabBtnSize, const Material* tabBtnBgMat, const RadioButtonWidget::SpriteSet tabSprites, TabsLocation tabsLocation, bool reorderable=false);
	ENGINEGUI_API ~TabView();

	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;

	ENGINEGUI_API void select(size_t newSelectionIndex);
	ENGINEGUI_API RadioButtonWidget* addItem(Widget* widget);
	//ENGINEGUI_API void removeItem(size_t which);
	//ENGINEGUI_API size_t getItemCount() const;
	//ENGINEGUI_API Widget* getItem(size_t which);
	//ENGINEGUI_API ButtonWidget* getItemTab(size_t which);
	//ENGINEGUI_API void setItem(size_t which, Widget* newWidget); // NOTE: Does NOT destroy old widget

	ENGINEGUI_API RadioButtonGroup* getTabArea();
	ENGINEGUI_API WidgetTransform* getContentTransform();

private:
	RadioButtonGroup* tabArea;
	WidgetTransform* contentTransform;
	bool reorderable;

	void tabSelectCallback(size_t from, size_t to);
};
