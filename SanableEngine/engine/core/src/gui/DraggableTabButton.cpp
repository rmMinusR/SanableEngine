#include "gui/DraggableTabButton.hpp"

#include "gui/HUD.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/TabView.hpp"

DraggableTabButton::DraggableTabButton(HUD* hud, Widget* content, RadioButtonGroup* group, SpriteSet sprites, const Material* material) :
	RadioButtonWidget(hud, group, sprites, material),
	content(content)
{
}

DraggableTabButton::~DraggableTabButton()
{
}

void DraggableTabButton::setDestinationFilter(std::function<bool(TabView*, TabView*)> fn)
{
	destinationFilter = fn;
}

void DraggableTabButton::moveTo(TabView* destination, size_t index)
{
	// Reparent tab
	group->detachItem(this);
	destination->getTabArea()->insertItem(this, -1);

	// Reparent content
	content->getTransform()->setParent(destination->getContentTransform());
}

void DraggableTabButton::onDragStarted(Vector2f dragStartPos, Vector2f currentMousePos)
{
	setState(ButtonState::Normal);
	updateSprite();

	assert(!dragIndicator);
	assert(!dragPositioner);
	dragIndicator = hud->addWidget<ImageWidget>(getMaterial(), sprites.normal);
	dragIndicator->getTransform()->setVisibility(WidgetVisibility::Visible & ~WidgetVisibility::FLAGS_Raycastable);
	dragIndicator->getTransform()->setRelativeRenderDepth(getTransform()->getRenderDepth() - 20); // Draw indicator on top of dragged button
	dragPositioner = dragIndicator->getTransform()->setPositioningStrategy<AnchoredPositioning>();
	dragPositioner->setCenterByOffsets(currentMousePos);
}

void DraggableTabButton::whileDragged(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f currentMousePos, Widget* currentlyHoveredWidget)
{
	if (dragStartWidget == this)
	{
		assert(dragIndicator);
		assert(dragPositioner);
		dragPositioner->setCenterByOffsets(currentMousePos);
	}
}

void DraggableTabButton::onDragFinished(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f dragEndPos, Widget* dragEndWidget)
{
	TabView* dst = nullptr;
	if (dragStartWidget == this && (dst = findTabView(dragEndPos)))
	{
		// Attempt to find TabView under cursor
		size_t numHits = hud->raycast(dragEndPos, nullptr, 0);
		WidgetTransform* hits[numHits];
		hud->raycast(dragEndPos, hits, numHits);

		assert(dragIndicator);
		assert(dragPositioner);

		group->detachItem(this);
		dst->getTabArea()->insertItem(this, -1);
		content->getTransform()->setParent(dst->getContentTransform());
		content->getTransform()->setChildIndex(this->getTransform()->getChildIndex());

		hud->destroyWidget(dragIndicator);
		dragIndicator = nullptr;
		dragPositioner = nullptr;
	}
}

TabView* DraggableTabButton::findTabView(Vector2f pos) const
{
	// List all at query pos
	size_t numHits = hud->raycast(pos, nullptr, 0);
	WidgetTransform* hits[numHits];
	hud->raycast(pos, hits, numHits);

	TabView* front = nullptr;
	WidgetTransform::depth_t frontDepth = std::numeric_limits<WidgetTransform::depth_t>::max();
	for (WidgetTransform* w : hits)
	{
		if (!front || w->getRenderDepth() < frontDepth)
		{
			if (TabView* v = dynamic_cast<TabView*>(w->getWidget()))
			{
				front = v;
				frontDepth = w->getRenderDepth();
			}
		}
	}

	return front;
}
