#include "gui/VerticalGroupWidget.hpp"

VerticalGroupWidget::VerticalGroupWidget(HUD* hud) :
	LinearLayoutGroupWidget(hud)
{
}

VerticalGroupWidget::~VerticalGroupWidget()
{
}

void VerticalGroupWidget::refreshLayout()
{
	std::vector<float> weights;
	for (int i = 0; i < getTransform()->getChildrenCount(); ++i)
	{
		PositioningStrategy* s = getTransform()->getChild(i)->getPositioningStrategy();
		weights.push_back(static_cast<AutoLayoutPositioning*>(s)->flexWeight.y);
	}

	std::vector<LayoutUtil::UIRect> rects = LayoutUtil::Stretch::vertical(getTransform()->getRect(), weights, padding);

	//Apply
	for (int i = 0; i < rects.size(); ++i) setRect(getTransform()->getChild(i), rects[i]);
}
