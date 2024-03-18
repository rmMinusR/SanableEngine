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
	updateWeightsCache();
	LayoutUtil::Stretch::horizontal(transform.getRect(), _weightsCache, padding);
}
