#include "gui/LinearLayoutGroupWidget.hpp"

#include "gui/LayoutUtil.hpp"

AutoLayoutPositioning::AutoLayoutPositioning(LinearLayoutGroupWidget* layout) :
	minSize(0, 0),
	preferredSize(0, 0),
	maxSize(FLT_MAX, FLT_MAX),
	flexWeight(1, 1),
	layout(layout)
{
}

AutoLayoutPositioning::~AutoLayoutPositioning()
{
}

void AutoLayoutPositioning::evaluate(Rect<float>* localRect_out, const WidgetTransform* transform)
{
	assert(layout == transform->getParent()->getWidget());

	if (isDirty()) layout->refreshLayout();
	
	//Sanity check
	for (int i = 0; i < transform->getChildrenCount(); ++i) assert(!transform->getChild(i)->isDirty());
}

bool AutoLayoutPositioning::isDirty() const
{
	if (layout->getTransform()->isDirty()) return true;

	for (int i = 0; i < layout->getTransform()->getChildrenCount(); ++i)
	{
		if (layout->getTransform()->getChild(i)->isDirty()) return true;
	}

	return false;
}

LinearLayoutGroupWidget::LinearLayoutGroupWidget(HUD* hud) :
	Widget(hud)
{
}

LinearLayoutGroupWidget::~LinearLayoutGroupWidget()
{
}

AutoLayoutPositioning* LinearLayoutGroupWidget::getPositioner(WidgetTransform* widget)
{
	return static_cast<AutoLayoutPositioning*>(widget->getPositioningStrategy());
}

const Material* LinearLayoutGroupWidget::getMaterial() const
{
	return nullptr;
}

void LinearLayoutGroupWidget::renderImmediate(Renderer* renderer)
{
}

void LinearLayoutGroupWidget::setRect(WidgetTransform* w, Rect<float> rect)
{
	w->rect = rect;

	w->localRect = rect;
	if (w->parent) w->localRect.topLeft -= w->parent->getRect().topLeft;
	w->dirty = false;
}
