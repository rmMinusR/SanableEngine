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
	updateWeightsCache();
	LayoutUtil::Stretch::vertical(transform.getRect(), _weightsCache, padding);
}
