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
			workingRect.topLeft.x, workingRect.bottomRight().x,
			nChildren, elements, locs, sizes
		);
	}

	//Apply
	for (size_t i = 0; i < nChildren; ++i)
	{
		setRectDirect(
			getTransform()->getChild(i),
			Rect<float>::fromMinMax(
				Vector2f(locs[i], workingRect.topLeft.y),
				Vector2f(locs[i]+sizes[i], workingRect.bottomRight().y)
			)
		);
	}
}
