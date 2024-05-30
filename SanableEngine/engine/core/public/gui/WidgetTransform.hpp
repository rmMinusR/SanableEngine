#pragma once

#include <optional>
#include <vector>
#include <functional>
#include <ReflectionSpec.hpp>
#include <glm/mat4x4.hpp>
#include "math/Vector2.inl"
#include "Anchor2D.inl"
#include "dllapi.h"
#include "math/Rect.inl"

/*
 
 o------------------o                 o-----------------o
 | Parent transform | --------------- | Child transform |
 o------------------o        |        o-----------------o
   < >                       |
    |             o----------------------o
    |             |     WidgetSocket     |
    |             | -------------------- |
    |    Updates  | positioningStrategy  | <>----.
    |        .--> | rect                 |       |
    |        |    o----------------------o       |
    |        |                                   |
    |        |                     o---------------------------o
    |        o-------------------- | << PositioningStrategy >> |
    |                              o---------------------------o
    | 0..1                                       ^
 o-------------o                                 |
 | LayoutGroup |  <---|            .-------------o-----------------.
 o-------------o      |            |             |                 |
                      |    o------------o   o----------o
                      |--- | AutoLayout |   | Anchored |  (user strategies...)
                           o------------o   o----------o

*/

struct WidgetTransform;
class PositioningStrategy;
class HUD;
class Widget;
class LinearLayoutGroupWidget;

//2D orthonormal affine transform
struct STIX_ENABLE_IMAGE_CAPTURE WidgetTransform
{
public:
	ENGINEGUI_API WidgetTransform(Widget* widget);
	ENGINEGUI_API ~WidgetTransform();
	//Trivially copyable and movable

	ENGINEGUI_API Rect<float> getRect() const;
	ENGINEGUI_API Rect<float> getLocalRect() const;
	ENGINEGUI_API PositioningStrategy* getPositioningStrategy() const;

	template<typename T, typename... TCtorArgs>
	T* setPositioningStrategy(TCtorArgs... ctorArgs)
	{
		static_assert(std::is_base_of_v<PositioningStrategy, T>);
		T* pos = widget->getHUD()->getMemory()->create<T>( std::forward(ctorArgs)... );
		setPositioningStrategy_internal(pos);
		return pos;
	}

	ENGINEGUI_API WidgetTransform* getParent() const;
	ENGINEGUI_API void setParent(WidgetTransform* parent);
	ENGINEGUI_API size_t getChildIndex() const;
	ENGINEGUI_API size_t getChildrenCount() const;
	ENGINEGUI_API WidgetTransform* getChild(size_t which) const;
	ENGINEGUI_API void visitChildren(const std::function<void(WidgetTransform*)>& visitor, bool recurse = false);

	typedef int depth_t;
	ENGINEGUI_API void setRenderDepth(depth_t depth);
	ENGINEGUI_API depth_t getRenderDepth() const;

	ENGINEGUI_API void setRelativeRenderDepth(depth_t depth);
	ENGINEGUI_API depth_t getRelativeRenderDepth() const;

	ENGINEGUI_API Widget* getWidget() const;
	ENGINEGUI_API bool isDirty() const;

	ENGINEGUI_API operator glm::mat4() const; //GL interop

private:
	ENGINEGUI_API void setPositioningStrategy_internal(PositioningStrategy*);


	Widget* widget;

	WidgetTransform* parent;
	std::vector<WidgetTransform*> children;
	size_t childIndex;
	depth_t relativeRenderDepth = 0;

	//Cached values
	mutable bool refreshing; //Acts as a canary in case PositioningStrategy does something stupid like call a dependent function mid-evaluate
	mutable bool dirty;
	mutable Rect<float> localRect; //Output of positioning strategy
	mutable Rect<float> rect; //Derived from localRect

	friend class HUD; //Needs to write rect/localrect on root
	friend class LinearLayoutGroupWidget; //FIXME awful workaround

	PositioningStrategy* positioningStrategy;

	void refresh() const;
	void markDirty() const;
};


typedef Anchor2D<float> UIAnchor;
class PositioningStrategy
{
public:
	ENGINEGUI_API PositioningStrategy();
	ENGINEGUI_API virtual ~PositioningStrategy();
	virtual void evaluate(Rect<float>* localRect_out, const WidgetTransform* transform) = 0;
};

class AnchoredPositioning : public PositioningStrategy
{
public:
	ENGINEGUI_API AnchoredPositioning();
	ENGINEGUI_API virtual ~AnchoredPositioning();
	ENGINEGUI_API virtual void evaluate(Rect<float>* localRect_out, const WidgetTransform* transform) override;

	UIAnchor minCorner;
	UIAnchor maxCorner;

	//Utility shorthand presets
	ENGINEGUI_API void fillParent(float padding = 0);
	ENGINEGUI_API void fillParentX(float padding = 0);
	ENGINEGUI_API void fillParentY(float padding = 0);
	ENGINEGUI_API void snapToCorner(Vector2f corner, Vector2f newSize); //Align own corner to specified corner in parent. If no new size is given, current size will be maintained.
	ENGINEGUI_API void setSizeByOffsets(Vector2f size, std::optional<Vector2f> pivot = std::nullopt); //Resize. If no pivot is given, the average of corner anchors will be used.
	ENGINEGUI_API void setCenterByOffsets(Vector2f pos, std::optional<Vector2f> pivot = std::nullopt); //Reposition. If no pivot is given, the average of corner anchors will be used.

	ENGINEGUI_API void setRectByOffsets(Rect<float> rect, WidgetTransform* parent);
	ENGINEGUI_API void setLocalRectByOffsets(const Rect<float>& rect, WidgetTransform* parent);
};
