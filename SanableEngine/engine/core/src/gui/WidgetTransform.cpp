#include "gui/WidgetTransform.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "gui/Widget.hpp"

WidgetTransform::WidgetTransform()
{
	parent = nullptr;
	minCorner = UIAnchor::centered();
	minCorner.offset -= Vector2f(50, 50);
	maxCorner = UIAnchor::centered();
	minCorner.offset += Vector2f(50, 50);
}

WidgetTransform::~WidgetTransform()
{
}

void WidgetTransform::fillParent(float padding)
{
	fillParentX(padding);
	fillParentY(padding);
}

void WidgetTransform::fillParentX(float padding)
{
	minCorner.ratio .x = 0;
	maxCorner.ratio .x = 1;
	minCorner.offset.x =  padding;
	maxCorner.offset.x = -padding;
}

void WidgetTransform::fillParentY(float padding)
{
	minCorner.ratio .y = 0;
	maxCorner.ratio .y = 1;
	minCorner.offset.y = padding;
	maxCorner.offset.y = -padding;
}

void WidgetTransform::snapToCorner(Vector2f corner, std::optional<Vector2f> size)
{
	Vector2f targetSize = size.value_or(getLocalRect().size);
	setMinCornerRatio(corner);
	setMaxCornerRatio(corner);
	setMinCornerOffset(targetSize * -corner);
	setMaxCornerOffset(targetSize * (Vector2f(1,1)-corner) );
}

void WidgetTransform::setSizeByOffsets(Vector2f size, std::optional<Vector2f> _pivot)
{
	Vector2f pivot = _pivot.value_or( (minCorner.ratio+maxCorner.ratio)/2 );
	setMinCornerOffset(size * pivot);
	setMaxCornerOffset(size * (Vector2f(1,1)-pivot) );
}

void WidgetTransform::setCenterByOffsets(Vector2f pos, std::optional<Vector2f> _pivot)
{
	Vector2f pivot = _pivot.value_or( (minCorner.ratio+maxCorner.ratio)/2 );
	Vector2f invPivot = Vector2f(1, 1)-pivot;
	Vector2f curPos = minCorner.offset*invPivot + maxCorner.offset*pivot;
	minCorner.offset += pos-curPos;
	maxCorner.offset += pos-curPos;
}

void WidgetTransform::setMinCornerRatio(const Vector2f& val, bool keepPosition)
{
	if (!keepPosition) minCorner.ratio = val;
	else minCorner.setRatioKeepingPosition(val, parent->getLocalRect().size);
}

void WidgetTransform::setMaxCornerRatio(const Vector2f& val, bool keepPosition)
{
	if (!keepPosition) maxCorner.ratio = val;
	else maxCorner.setRatioKeepingPosition(val, parent->getLocalRect().size);
}

void WidgetTransform::setMinCornerOffset(const Vector2f& val, bool keepPosition)
{
	if (!keepPosition) minCorner.offset = val;
	else minCorner.setOffsetKeepingPosition(val, parent->getLocalRect().size);
}

void WidgetTransform::setMaxCornerOffset(const Vector2f& val, bool keepPosition)
{
	if (!keepPosition) maxCorner.offset = val;
	else maxCorner.setOffsetKeepingPosition(val, parent->getLocalRect().size);
}

Rect<float> WidgetTransform::getRect() const
{
	Rect rect = getLocalRect();
	if (parent) rect.topLeft += parent->getRect().topLeft;
	return rect;
}

void WidgetTransform::setRectByOffsets(Rect<float> rect)
{
	if (parent) rect.topLeft -= parent->getRect().topLeft;
	setLocalRectByOffsets(rect);
}

Rect<float> WidgetTransform::getLocalRect() const
{
	Vector2f parentSize = parent ? parent->getLocalRect().size : Vector2f(0,0);
	Vector2f localMin = minCorner.calcAnchor(parentSize);
	Vector2f localMax = maxCorner.calcAnchor(parentSize);
	return Rect<float>::fromMinMax(localMin, localMax);
}

void WidgetTransform::setLocalRectByOffsets(const Rect<float>& rect)
{
	minCorner.offset.zero();
	maxCorner.offset.zero();
	Vector2f parentSize = parent ? parent->getLocalRect().size : Vector2f(0,0);
	minCorner.offset = rect.topLeft       - minCorner.calcAnchor(parentSize);
	maxCorner.offset = rect.bottomRight() - maxCorner.calcAnchor(parentSize);
}

/*
float WidgetTransform::getScale() const
{
	if (parent) return localScale * parent->getScale();
	else return localScale;
}

void WidgetTransform::setScale(float val)
{
	localScale = val;
	if (parent) localScale /= parent->getScale();
}

float WidgetTransform::getLocalScale() const
{
	return localScale;
}

void WidgetTransform::setLocalScale(float val)
{
	localScale = val;
}
*/

WidgetTransform* WidgetTransform::getParent() const
{
	return parent;
}

void WidgetTransform::setParent(WidgetTransform* parent)
{
	//Sanity check: can't cause loops
	{
		WidgetTransform const* i = this->parent;
		while (i)
		{
			assert(i != this);
			i = i->parent;
		}
	}

	//Remove self from parent's children list
	if (this->parent)
	{
		auto it = std::find(this->parent->children.begin(), this->parent->children.end(), this);
		if (it != this->parent->children.end()) this->parent->children.erase(it);
	}

	this->parent = parent;

	//Add self to parent's children list
	if (parent)
	{
		assert(std::find(parent->children.begin(), parent->children.end(), this) == parent->children.end());
		parent->children.push_back(this);
	}
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

Widget* WidgetTransform::getWidget() const
{
	return (Widget*)const_cast<char*>( reinterpret_cast<const char*>(this)-offsetof(Widget, transform) ); //FIXME awful hack, but will be useful later when widget transforms occupy separate memory
}
