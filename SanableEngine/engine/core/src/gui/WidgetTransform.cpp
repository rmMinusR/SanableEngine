#include "gui/WidgetTransform.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

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

void WidgetTransform::fillParent()
{
	setMinCornerRatio ({ 0, 0 });
	setMaxCornerRatio ({ 1, 1 });
	setMinCornerOffset({ 0, 0 });
	setMaxCornerOffset({ 0, 0 });
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
		WidgetTransform* i = this->parent;
		while (i)
		{
			assert(i != this);
			i = i->parent;
		}
	}

	this->parent = parent;
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
