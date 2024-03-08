#include "gui/WidgetTransform.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

WidgetTransform::WidgetTransform()
{
}

void WidgetTransform::setPivot(float x, float y)
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
	r.x = offsetX - width*pivotX;
	r.y = offsetY - height*pivotY;
	if (parent)
	{
		r.x += parent->width  * parent->anchorX;
		r.y += parent->height * parent->anchorY;
	}
	r.width = width;
	r.height = height;
	return r;
}

void WidgetTransform::setLocalRect(const Rect<float>& rect)
{
	width = rect.width;
	height = rect.height;
	offsetX = rect.x + width*pivotX;
	offsetY = rect.y + height*pivotY;
	if (parent)
	{
		offsetX -= parent->width  * parent->anchorX;
		offsetY -= parent->height * parent->anchorY;
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
	Rect<float> rect;
	return glm::translate<float, glm::packed_highp>(
			glm::identity<glm::mat4>(),
			glm::vec3(rect.x, rect.y, getRenderDepth())
		)
		//TODO rotation component goes here
		* glm::scale(glm::identity<glm::mat4>(), glm::vec3(getScale()));
}
