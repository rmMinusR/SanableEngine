#pragma once

#include <unordered_map>
#include "Widget.hpp"
#include "LayoutUtil.hpp"

class AutoLayoutPositioning;

//Helper class for VerticalGroupWidget and HorizontalGroupWidget
class LinearLayoutGroupWidget : public Widget
{
protected:
	virtual void refreshLayout() = 0; //Must set rect for all children
	friend class AutoLayoutPositioning;

	ENGINEGUI_API static void setRect(WidgetTransform* w, Rect<float> rect); //Horrible fix
public:
	ENGINEGUI_API LinearLayoutGroupWidget(HUD* hud);
	ENGINEGUI_API virtual ~LinearLayoutGroupWidget();

	ENGINEGUI_API AutoLayoutPositioning* getPositioner(WidgetTransform* widget);
	LayoutUtil::Padding padding;

	ENGINEGUI_API virtual const Material* getMaterial() const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer) override;
};

class AutoLayoutPositioning : public PositioningStrategy
{
public:
	ENGINEGUI_API AutoLayoutPositioning(LinearLayoutGroupWidget* layout);
	ENGINEGUI_API virtual ~AutoLayoutPositioning();
	ENGINEGUI_API virtual void evaluate(Rect<float>* localRect_out, const WidgetTransform* transform) override;

	ENGINEGUI_API bool isDirty() const;

	Vector2f minSize; //Satisfied first
	Vector2f preferredSize; //Satisfied second, in a 1:1 ratio
	Vector2f maxSize; //Satisfied last, in accordance with flexWeight
	Vector2f flexWeight;

	LinearLayoutGroupWidget* layout;
};
