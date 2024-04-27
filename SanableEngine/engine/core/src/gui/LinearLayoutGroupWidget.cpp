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
	//Clear and make space
	_weightsCache.clear();
	if (_weightsCache.capacity() < transform.getChildrenCount()) _weightsCache.reserve(transform.getChildrenCount());
	
	//Repopulate
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

const Material* LinearLayoutGroupWidget::getMaterial() const
{
	return nullptr;
}

void LinearLayoutGroupWidget::renderImmediate(Renderer* renderer)
{
}
