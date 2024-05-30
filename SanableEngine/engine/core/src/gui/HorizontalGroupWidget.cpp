#include "gui/HorizontalGroupWidget.hpp"

HorizontalGroupWidget::HorizontalGroupWidget(HUD* hud) :
	LinearLayoutGroupWidget(hud)
{
}

HorizontalGroupWidget::~HorizontalGroupWidget()
{
}

void HorizontalGroupWidget::refreshLayout()
{
	std::vector<float> weights;
	for (int i = 0; i < getTransform()->getChildrenCount(); ++i)
	{
		PositioningStrategy* s = getTransform()->getChild(i)->getPositioningStrategy();
		weights.push_back(static_cast<AutoLayoutPositioning*>(s)->flexWeight.x);
	}

	std::vector<LayoutUtil::UIRect> rects = LayoutUtil::Stretch::horizontal(getTransform()->getRect(), weights, padding);

	//Apply
	for (int i = 0; i < rects.size(); ++i) setRect(getTransform()->getChild(i), rects[i]);
}
