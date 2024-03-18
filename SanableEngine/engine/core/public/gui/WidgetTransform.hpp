#pragma once

#include <optional>
#include <vector>
#include <functional>
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

	/*
	struct CornerAnchorForm;
	struct AnchorPivotForm;

	struct CornerAnchorForm
	{
		UIAnchor minCorner;
		UIAnchor maxCorner;

		ENGINEGUI_API void toAnchorPivotForm(const Vector2f parentSize, AnchorPivotForm* out);
	};

	struct AnchorPivotForm
	{
		UIAnchor anchor;
		Vector2f pivot; //Range 0-1
		Vector2f flatSize; //Pixels
		Vector2f inheritedSize; //Range 0-1

		ENGINEGUI_API void toCornerForm(const Vector2f parentSize, CornerAnchorForm* out);
	};
	*/

private:
	UIAnchor minCorner;
	UIAnchor maxCorner;
	//UIAnchor pivot;
	//float localScale = 1;
	depth_t relativeRenderDepth = 0;
	WidgetTransform* parent;
	std::vector<WidgetTransform*> children;

	//TODO cache
public:
	ENGINEGUI_API WidgetTransform();
	ENGINEGUI_API ~WidgetTransform();
	//Trivially copyable and movable

	//Utility shorthand presets
	ENGINEGUI_API void fillParent(float padding = 0);
	ENGINEGUI_API void fillParentX(float padding = 0);
	ENGINEGUI_API void fillParentY(float padding = 0);
	ENGINEGUI_API void snapToCorner(Vector2f corner, std::optional<Vector2f> newSize = std::nullopt); //Align own corner to specified corner in parent. If no new size is given, current size will be maintained.
	ENGINEGUI_API void setSizeByOffsets(Vector2f size, std::optional<Vector2f> pivot = std::nullopt); //Resize. If no pivot is given, the average of corner anchors will be used.
	ENGINEGUI_API void setCenterByOffsets(Vector2f pos, std::optional<Vector2f> pivot = std::nullopt); //Reposition. If no pivot is given, the average of corner anchors will be used.

	//Fine control
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

	ENGINEGUI_API WidgetTransform* getParent() const;
	ENGINEGUI_API void setParent(WidgetTransform* parent);
	ENGINEGUI_API size_t getChildrenCount() const;
	ENGINEGUI_API WidgetTransform* getChild(size_t which) const;
	ENGINEGUI_API void visitChildren(const std::function<void(WidgetTransform*)>& visitor, bool recurse = false);

	ENGINEGUI_API void setRenderDepth(depth_t depth);
	ENGINEGUI_API depth_t getRenderDepth() const;

	ENGINEGUI_API void setRelativeRenderDepth(depth_t depth);
	ENGINEGUI_API depth_t getRelativeRenderDepth() const;

	ENGINEGUI_API operator glm::mat4() const; //GL interop
};
