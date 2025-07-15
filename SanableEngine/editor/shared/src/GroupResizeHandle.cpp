#include "GroupResizeHandle.hpp"

#include <algorithm>

#include "gui/ImageWidget.hpp"
#include "gui/HorizontalGroupWidget.hpp"
#include "gui/VerticalGroupWidget.hpp"

GroupResizeHandle::GroupResizeHandle(HUD* hud, ImageWidget* background, const UISprite* spriteNormal, const UISprite* spriteDragged) :
	Widget(hud),
	background(background),
	socket(hud, this),
	spriteNormal(spriteNormal),
	spriteDragged(spriteDragged)
{
	getTransform()->setRelativeRenderDepth(1);
	socket.setRelativeRenderDepth(-1);
	socket.put(background);
}

GroupResizeHandle::~GroupResizeHandle()
{
}

WidgetSocket* GroupResizeHandle::getSocket()
{
	return &socket;
}

const WidgetSocket* GroupResizeHandle::getSocket() const
{
	return &socket;
}

const Material* GroupResizeHandle::getMaterial() const
{
	return nullptr;
}

void GroupResizeHandle::renderImmediate(Renderer* renderer)
{
}

void GroupResizeHandle::onDragStarted(Vector2f dragStartPos, Vector2f currentMousePos)
{
	if (spriteDragged)
	{
		background->setSprite(spriteDragged);
	}
}

void GroupResizeHandle::whileDragged(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f currentMousePos, Widget* currentlyHoveredWidget)
{
	if (dragStartWidget != this) return; // Only resize when drag started on self

	WidgetTransform* parent = getTransform()->getParent();

	float (*axis)(Vector2<float>) = nullptr;
	if (dynamic_cast<HorizontalGroupWidget*>(parent->getWidget()))
	{
		axis = +[](Vector2<float> v) -> float { return v.x; };
	}
	else if (dynamic_cast<VerticalGroupWidget*>(parent->getWidget()))
	{
		axis = +[](Vector2<float> v) -> float { return v.y; };
	}
	else
	{
		assert(false && "GroupResizeHandle only allowed inside a LinearLayoutGroup");
		return;
	}

	WidgetTransform* before = parent->getChild(getTransform()->getChildIndex() - 1);
	WidgetTransform* after  = parent->getChild(getTransform()->getChildIndex() + 1);
	AutoLayoutPositioning* posBefore = static_cast<AutoLayoutPositioning*>(before->getPositioningStrategy());
	AutoLayoutPositioning* posAfter  = static_cast<AutoLayoutPositioning*>(after->getPositioningStrategy());

	float controlledAreaLo = std::min(axis(before->getRect().topLeft), axis(after->getRect().topLeft)) + posBefore->config.getMinSize();
	float controlledAreaSize = axis(before->getRect().size) + axis(after->getRect().size) - posBefore->config.getMinSize() - posAfter->config.getMinSize();

	float mousePosInArea = axis(currentMousePos) - controlledAreaLo;
	float ownSize = axis(getTransform()->getRect().size);
	float flexRatio = (mousePosInArea - ownSize/2) / controlledAreaSize;
	flexRatio = std::clamp<float>(flexRatio, 0, 1);

	float flexWeightTotal = posBefore->config.flexWeight + posAfter->config.flexWeight;
	posBefore->config.flexWeight = flexWeightTotal * flexRatio;
	posAfter ->config.flexWeight = flexWeightTotal * (1-flexRatio);

	getTransform()->getParent()->markDirty();
}

void GroupResizeHandle::onDragFinished(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f dragEndPos, Widget* dragEndWidget)
{
	if (spriteNormal)
	{
		background->setSprite(spriteNormal);
	}
}
