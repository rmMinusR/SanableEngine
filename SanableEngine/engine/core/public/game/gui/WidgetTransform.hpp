#pragma once

#include <glm/mat4x4.hpp>
#include "dllapi.h"
#include "Rect.hpp"

/// <summary>
/// 2D affine transform but preserves distances and angles
/// </summary>
/// <remarks>
/// 
/// </remarks>
struct WidgetTransform
{
public:
	typedef int depth_t;

private:
	float anchorX = 0, anchorY = 0;
	float offsetX = 0, offsetY = 0;
	float width = 100, height = 100;
	float pivotX = 0.5f, pivotY = 0.5f;
	float localScale = 1;
	depth_t relativeRenderDepth = 0;
	WidgetTransform* parent = nullptr;

	//TODO cache
public:
	ENGINEGUI_API WidgetTransform();
	ENGINEGUI_API ~WidgetTransform() = default;
	//Trivially copyable and movable

	/// <summary>
	/// Change the transform's pivot. This will change the rendered rect.
	/// </summary>
	/// <param name="x">Typical range 0-1</param>
	/// <param name="y">Typical range 0-1</param>
	ENGINEGUI_API void setPivot(float x, float y);

	/// <summary>
	/// Change the transform's position in parent.
	/// </summary>
	/// <param name="anchorX">Typical range 0-1</param>
	/// <param name="anchorY">Typical range 0-1</param>
	/// <param name="offsetX">Raw pixel value</param>
	/// <param name="offsetY">Raw pixel value</param>
	ENGINEGUI_API void setLocalPosition(float anchorX, float anchorY, float offsetX, float offsetY);

	ENGINEGUI_API Rect<float> getRect() const;
	ENGINEGUI_API void setRect(Rect<float> rect);

	ENGINEGUI_API Rect<float> getLocalRect() const;
	ENGINEGUI_API void setLocalRect(const Rect<float>& rect);

	ENGINEGUI_API float getScale() const;
	ENGINEGUI_API void setScale(float val);
	
	ENGINEGUI_API float getLocalScale() const;
	ENGINEGUI_API void setLocalScale(float val);

	ENGINEGUI_API WidgetTransform* getParent() const;
	ENGINEGUI_API void setParent(WidgetTransform* parent);

	ENGINEGUI_API void setRenderDepth(depth_t depth);
	ENGINEGUI_API depth_t getRenderDepth() const;

	ENGINEGUI_API void setRelativeRenderDepth(depth_t depth);
	ENGINEGUI_API depth_t getRelativeRenderDepth() const;

	ENGINEGUI_API operator glm::mat4() const; //GL interop
};
