#include "gui/LinearLayoutGroupWidget.hpp"

#include "gui/LayoutUtil.hpp"

LinearLayoutGroupWidget::LinearLayoutGroupWidget(HUD* hud) :
	Widget(hud)
{
}

LinearLayoutGroupWidget::~LinearLayoutGroupWidget()
{
}


void LinearLayoutGroupWidget::updateWeightsCache()
{
	//Clear former children that aren't children anymore
	//TODO probably inefficient
	_weightsCache.erase(std::find_if(
		_weightsCache.end(),
		_weightsCache.end(),
		[&](std::pair<WidgetTransform*, float> i) { return i.first->getParent() != &transform; }
	));

	//Clear weightsHelper
	_weightsCache.clear();
	if (_weightsCache.capacity() < transform.getChildrenCount()) _weightsCache.resize(transform.getChildrenCount());
	
	//Repopulate weightsHelper
	transform.visitChildren([&](WidgetTransform* i)
	{
		auto it = customWeights.find(i);
		float weight = (it != customWeights.end()) ? it->second : 1;
		_weightsCache.push_back({ i, weight });
	});
}

void LinearLayoutGroupWidget::setFlexWeight(WidgetTransform* widget, float weight)
{
	customWeights[widget] = weight;
}
