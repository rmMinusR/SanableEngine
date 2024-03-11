#pragma once

#include <ReflectionSpec.hpp>
#include <glm/mat4x4.hpp>
#include "Vector2.inl"
#include "Anchor2D.inl"
#include "dllapi.h"
#include "Rect.hpp"

typedef Anchor2D<float> UIAnchor;


//2D affine transform but preserves distances and angles
struct STIX_ENABLE_IMAGE_CAPTURE WidgetTransform
{
public:
	typedef int depth_t;

private:
	UIAnchor minCorner;
	UIAnchor maxCorner;
	//UIAnchor pivot;
	//float localScale = 1;
	depth_t relativeRenderDepth = 0;
	WidgetTransform const* parent;

	//TODO cache
public:
	ENGINEGUI_API WidgetTransform();
	ENGINEGUI_API ~WidgetTransform();
	//Trivially copyable and movable

	//Utility shorthand presets
	ENGINEGUI_API void fillParent();


	ENGINEGUI_API void setMinCornerRatio(const Vector2f& val, bool keepPosition = false);
	ENGINEGUI_API void setMaxCornerRatio(const Vector2f& val, bool keepPosition = false);
	ENGINEGUI_API void setMinCornerOffset(const Vector2f& val, bool keepPosition = false);
	ENGINEGUI_API void setMaxCornerOffset(const Vector2f& val, bool keepPosition = false);

	ENGINEGUI_API Rect<float> getRect() const;
	ENGINEGUI_API void setRectByOffsets(Rect<float> rect);

	ENGINEGUI_API Rect<float> getLocalRect() const;
	ENGINEGUI_API void setLocalRectByOffsets(const Rect<float>& rect);

	//ENGINEGUI_API float getScale() const;
	//ENGINEGUI_API void setScale(float val);
	//ENGINEGUI_API float getLocalScale() const;
	//ENGINEGUI_API void setLocalScale(float val);

	ENGINEGUI_API WidgetTransform const* getParent() const;
	ENGINEGUI_API void setParent(WidgetTransform const* parent);

	ENGINEGUI_API void setRenderDepth(depth_t depth);
	ENGINEGUI_API depth_t getRenderDepth() const;

	ENGINEGUI_API void setRelativeRenderDepth(depth_t depth);
	ENGINEGUI_API depth_t getRelativeRenderDepth() const;

	ENGINEGUI_API operator glm::mat4() const; //GL interop
};
