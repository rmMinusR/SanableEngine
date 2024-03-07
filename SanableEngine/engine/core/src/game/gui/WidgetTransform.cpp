#include "game/gui/WidgetTransform.hpp"

WidgetTransform::WidgetTransform()
{
}

void WidgetTransform::setPivot(float nrmX, float nrmY)
{
	pivotX = x;
	pivotY = y;
}

void WidgetTransform::setLocalPosition(float anchorX, float anchorY, float offsetX, float offsetY)
{
	this->anchorX = anchorX;
	this->anchorY = anchorY;
	this->offsetX = offsetX;
	this->offsetY = offsetY;
}

Rect<float> WidgetTransform::getRect() const
{
	Rect rect = getLocalRect();
	if (parent)
	{
		Rect<float> parentRect = parent->getRect();
		rect.x += parentRect.x;
		rect.y += parentRect.y;
	}
	return rect;
}

void WidgetTransform::setRect(Rect<float> rect)
{
	if (parent)
	{
		Rect<float> parentRect = parent->getRect();
		rect.x -= parentRect.x;
		rect.y -= parentRect.y;
	}
	setLocalRect(rect);
}

Rect<float> WidgetTransform::getLocalRect() const
{
	Rect<float> r;
	r.x = offsetX + width*pivotX;
	r.y = offsetY + height*pivotY;
	if (parent)
	{
		r.x += parent->offsetX * parent->anchorX;
		r.y += parent->offsetY * parent->anchorY;
	}
	r.width = width;
	r.height = height;
	return r;
}

void WidgetTransform::setLocalRect(const Rect<float>& rect)
{
	width = rect.width;
	height = rect.height;
	rootX = rect.x - width*pivotX;
	rootY = rect.y - height*pivotY;
	if (parent)
	{
		r.x -= parent->offsetX * parent->anchorX;
		r.y -= parent->offsetY * parent->anchorY;
	}
}

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

WidgetTransform* WidgetTransform::getParent() const
{
	return parent;
}

void WidgetTransform::setParent(WidgetTransform* parent)
{
	this->parent = parent;
}

void WidgetTransform::setRenderDepth(depth_t depth)
{
	relativeRenderDepth = depth;
	if (parent) relativeRenderDepth -= parent->getRelativeRenderDepth();
}

depth_t WidgetTransform::getRenderDepth() const
{
	if (parent) return relativeRenderDepth + parent->getRenderDepth();
	return relativeRenderDepth;
}

void WidgetTransform::setRelativeRenderDepth(depth_t depth)
{
	relativeRenderDepth = depth;
}

depth_t WidgetTransform::getRelativeRenderDepth() const
{
	return relativeRenderDepth;
}
