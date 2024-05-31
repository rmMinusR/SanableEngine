#include "gui/LinearLayoutGroupWidget.hpp"

#include "gui/LayoutUtil.hpp"

AutoLayoutPositioning::AutoLayoutPositioning(LinearLayoutGroupWidget* layout) :
	minSize(0),
	preferredSize(0),
	maxSize(FLT_MAX),
	flexWeight(1),
	layout(layout)
{
}

AutoLayoutPositioning::~AutoLayoutPositioning()
{
}

void AutoLayoutPositioning::evaluate(Rect<float>* localRect_out, const WidgetTransform* transform)
{
	assert(layout->getTransform() == transform->getParent());

	if (isDirty(layout->getTransform())) layout->refreshLayout();
	
	//Sanity check
	for (int i = 0; i < layout->getTransform()->getChildrenCount(); ++i) assert(!layout->getTransform()->getChild(i)->isDirty());
}

bool AutoLayoutPositioning::isDirty(const WidgetTransform* t)
{
	if (t->isDirty()) return true;

	for (int i = 0; i < t->getChildrenCount(); ++i)
	{
		if (t->getChild(i)->isDirty()) return true;
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

void LinearLayoutGroupWidget::setRectDirect(WidgetTransform* w, Rect<float> rect)
{
	w->rect = rect;

	w->localRect = rect;
	if (w->parent) w->localRect.topLeft -= w->parent->getRect().topLeft;
	w->dirty = false;
}
