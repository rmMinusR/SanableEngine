#include "gui/WidgetTransform.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "gui/Widget.hpp"
#include "gui/HUD.hpp"

void WidgetTransform::refresh() const
{
	assert(!refreshing && "Cannot call functions dependent on updated layout while refreshing that layout");
	refreshing = true;
	
	//Ensure parent is up to date
	if (parent && parent->dirty) parent->refresh();

	//Run positioning strategy, if present
	if (positioningStrategy) positioningStrategy->evaluate(&localRect, this);

	//Calculate globals from locals
	rect.topLeft = localRect.topLeft;
	if (parent) rect.topLeft += parent->rect.topLeft;
	rect.size = localRect.size;

	//Sanity check
	if (parent) assert(parent->rect.contains(rect.topLeft) && parent->rect.contains(rect.bottomRight()));

	dirty = false;
	refreshing = false;
}

void WidgetTransform::markDirty() const
{
	//Any non-dirty transforms will always have non-dirty parents all the way to the root
	//So if this node is dirty, we can guarantee all children are also still dirty
	if (dirty) return;

	dirty = true;
	for (const WidgetTransform* w : children) w->markDirty();
}

WidgetTransform::WidgetTransform(Widget* widget, HUD* hud)
{
	this->hud = hud;
	this->widget = widget;
	parent = nullptr;

	dirty = true;
	refreshing = false;

	positioningStrategy = nullptr;
	setPositioningStrategy<AnchoredPositioning>();
}

WidgetTransform::~WidgetTransform()
{
	if (positioningStrategy) widget->getHUD()->getMemory()->destroy(positioningStrategy);
}

Rect<float> WidgetTransform::getRect() const
{
	if (dirty) refresh();
	return rect;
}

Rect<float> WidgetTransform::getLocalRect() const
{
	if (dirty) refresh();
	return localRect;
}

PositioningStrategy* WidgetTransform::getPositioningStrategy() const
{
	return positioningStrategy;
}

void WidgetTransform::setPositioningStrategy_internal(PositioningStrategy* _new)
{
	assert(!_new || _new != positioningStrategy);
	
	markDirty();
	if (positioningStrategy) hud->getMemory()->destroy(positioningStrategy);
	positioningStrategy = _new;
}

WidgetTransform* WidgetTransform::getParent() const
{
	return parent;
}

void WidgetTransform::setParent(WidgetTransform* parent)
{
	if (this->parent == parent) return;

	//Sanity check: can't cause loops
	{
		WidgetTransform const* i = parent;
		while (i)
		{
			assert(i != this);
			i = i->parent;
		}
	}

	//Remove self from parent's children list
	if (parent)
	{
		auto it = std::find(parent->children.begin(), parent->children.end(), this);
		if (it != parent->children.end()) parent->children.erase(it);
	}

	this->parent = parent;
	markDirty();

	//Add self to parent's children list
	if (parent)
	{
		assert(std::find(parent->children.begin(), parent->children.end(), this) == parent->children.end());
		childIndex = parent->children.size();
		parent->children.push_back(this);
	}
}

size_t WidgetTransform::getChildIndex() const
{
	return parent ? childIndex : -1;
}

size_t WidgetTransform::getChildrenCount() const
{
	return children.size();
}

WidgetTransform* WidgetTransform::getChild(size_t which) const
{
	return children[which];
}

void WidgetTransform::visitChildren(const std::function<void(WidgetTransform*)>& visitor, bool recurse)
{
	for (WidgetTransform* i : children)
	{
		visitor(i);
		if (recurse) i->visitChildren(visitor, recurse);
	}
}

void WidgetTransform::setRenderDepth(depth_t depth)
{
	relativeRenderDepth = depth;
	if (parent) relativeRenderDepth -= parent->getRelativeRenderDepth();
}

WidgetTransform::depth_t WidgetTransform::getRenderDepth() const
{
	if (parent) return relativeRenderDepth + parent->getRenderDepth();
	return relativeRenderDepth;
}

void WidgetTransform::setRelativeRenderDepth(depth_t depth)
{
	relativeRenderDepth = depth;
}

WidgetTransform::depth_t WidgetTransform::getRelativeRenderDepth() const
{
	return relativeRenderDepth;
}

Widget* WidgetTransform::getWidget() const
{
	return widget;
}

bool WidgetTransform::isDirty() const
{
	return dirty;
}

WidgetTransform::operator glm::mat4() const
{
	Rect<float> rect = getRect();
	return glm::translate<float, glm::packed_highp>(
			glm::identity<glm::mat4>(),
			glm::vec3(rect.topLeft.x, rect.topLeft.y, getRenderDepth())
		);
		//TODO rotation component goes here
		//* glm::scale(glm::identity<glm::mat4>(), glm::vec3(getScale())); //TODO scale component goes here
}

PositioningStrategy::PositioningStrategy()
{
}

PositioningStrategy::~PositioningStrategy()
{
}

AnchoredPositioning::AnchoredPositioning()
{
	minCorner = UIAnchor::centered();
	minCorner.offset -= Vector2f(50, 50);
	maxCorner = UIAnchor::centered();
	minCorner.offset += Vector2f(50, 50);
}

AnchoredPositioning::~AnchoredPositioning()
{
}

void AnchoredPositioning::evaluate(Rect<float>* localRect_out, const WidgetTransform* transform)
{
	if (!transform->getParent()) return;

	Rect<float> parentRect = transform->getParent()->getRect();
	*localRect_out = Rect<float>::fromMinMax(minCorner.calcAnchor(parentRect.size), maxCorner.calcAnchor(parentRect.size));
}

void AnchoredPositioning::fillParent(float padding)
{
	fillParentX(padding);
	fillParentY(padding);
}

void AnchoredPositioning::fillParentX(float padding)
{
	minCorner.ratio .x = 0;
	maxCorner.ratio .x = 1;
	minCorner.offset.x =  padding;
	maxCorner.offset.x = -padding;
}

void AnchoredPositioning::fillParentY(float padding)
{
	minCorner.ratio .y = 0;
	maxCorner.ratio .y = 1;
	minCorner.offset.y = padding;
	maxCorner.offset.y = -padding;
}

void AnchoredPositioning::snapToCorner(Vector2f corner, Vector2f size)
{
	minCorner.ratio = corner;
	maxCorner.ratio = corner;
	minCorner.offset = size * -corner;
	maxCorner.offset = size * (Vector2f(1,1)-corner);
}

void AnchoredPositioning::setSizeByOffsets(Vector2f size, std::optional<Vector2f> _pivot)
{
	Vector2f pivot = _pivot.value_or( (minCorner.ratio+maxCorner.ratio)/2 );
	minCorner.offset = size * pivot;
	maxCorner.offset = size * (Vector2f(1,1)-pivot);
}

void AnchoredPositioning::setCenterByOffsets(Vector2f pos, std::optional<Vector2f> _pivot)
{
	Vector2f pivot = _pivot.value_or( (minCorner.ratio+maxCorner.ratio)/2 );
	Vector2f invPivot = Vector2f(1, 1)-pivot;
	Vector2f curPos = minCorner.offset*invPivot + maxCorner.offset*pivot;
	minCorner.offset += pos-curPos;
	maxCorner.offset += pos-curPos;
}

void AnchoredPositioning::setRectByOffsets(Rect<float> rect, WidgetTransform* parent)
{
	if (parent) rect.topLeft -= parent->getRect().topLeft;
	setLocalRectByOffsets(rect, parent);
}

void AnchoredPositioning::setLocalRectByOffsets(const Rect<float>& rect, WidgetTransform* parent)
{
	minCorner.offset.zero();
	maxCorner.offset.zero();
	Vector2f parentSize = parent ? parent->getLocalRect().size : Vector2f(0, 0);
	minCorner.offset = rect.topLeft - minCorner.calcAnchor(parentSize);
	maxCorner.offset = rect.bottomRight() - maxCorner.calcAnchor(parentSize);
}
