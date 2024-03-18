#pragma once

#include <unordered_map>
#include "Widget.hpp"
#include "LayoutUtil.hpp"

//Helper class for VerticalGroupWidget and HorizontalGroupWidget
class LinearLayoutGroupWidget : public Widget
{
protected:
	std::unordered_map<WidgetTransform*, float> customWeights;
	std::vector<std::pair<WidgetTransform*, float>> _weightsCache; //Cached so we aren't constantly making heap allocations
	ENGINEGUI_API void updateWeightsCache();

public:
	ENGINEGUI_API LinearLayoutGroupWidget(HUD* hud);
	ENGINEGUI_API virtual ~LinearLayoutGroupWidget();

	virtual void refreshLayout() override = 0;

	ENGINEGUI_API void setFlexWeight(WidgetTransform* widget, float weight);
	LayoutUtil::Padding padding;
};
