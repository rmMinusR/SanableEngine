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
	size_t nChildren = getTransform()->getChildrenCount();

	float locs[nChildren];
	float sizes[nChildren];

	Rect<float> workingRect = getTransform()->getRect();

	{
		//Arrange
		LayoutUtil::LinearElementView elements[nChildren];
		for (size_t i = 0; i < nChildren; ++i)
		{
			const AutoLayoutPositioning* src = getPositioner(getTransform()->getChild(i));
			elements[i].minSize       = src->minSize;
			elements[i].preferredSize = src->preferredSize;
			elements[i].maxSize       = src->maxSize;
			elements[i].flexWeight    = src->flexWeight;
		}

		//Act
		LayoutUtil::linear(
			workingRect.topLeft.y, workingRect.bottomRight().y,
			nChildren, elements, locs, sizes
		);
	}

	//Apply
	for (size_t i = 0; i < nChildren; ++i)
	{
		setRectDirect(
			getTransform()->getChild(i),
			Rect<float>::fromMinMax(
				Vector2f(workingRect.topLeft.x, locs[i]),
				Vector2f(workingRect.bottomRight().x, locs[i]+sizes[i])
			)
		);
	}
}
