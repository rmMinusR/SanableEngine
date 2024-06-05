#pragma once

#include <optional>
#include <vector>
#include <functional>
#include <utility>
#include <ReflectionSpec.hpp>
#include <glm/mat4x4.hpp>
#include "math/Vector2.inl"
#include "Anchor2D.inl"
#include "dllapi.h"
#include "math/Rect.inl"

/*
 
 o------------------o             o---------------------o
 | Parent transform | ----------- |   Child transform   |
 o------------------o             | ------------------- |
       < >                        | rect                | <----\
        |                         | positioningStrategy |      |  Writes to
        |                         o---------------------o      |
        |                                   < >                |
        |                                    |                 |
        |                                    |                 |
        |                      o---------------------------o   |
        |                      | << PositioningStrategy >> | --/
        |                      o---------------------------o
        |                                    ^
        |                                    |
        |                      /-------------o-----------------\
        | 0..1                 |             |                 |
 o-------------o       o------------o   o----------o
 | LayoutGroup | <---- | AutoLayout |   | Anchored |  (user strategies...)
 o-------------o       o------------o   o----------o

*/

struct WidgetTransform;
class PositioningStrategy;
class HUD;
class Widget;
class LinearLayoutGroupWidget;

//2D orthonormal affine transform
struct STIX_ENABLE_IMAGE_CAPTURE WidgetTransform
{
private:
	ENGINEGUI_API void setPositioningStrategy_internal(PositioningStrategy*);

public:
	ENGINEGUI_API WidgetTransform(Widget* widget, HUD* hud);
	ENGINEGUI_API ~WidgetTransform();
	//FIXME needs proper copy ctor to deal with children tracking. Default-generated move ctor is ok but cross-module would be best.
	//Alternatively, could rework children tracking so children only reference their parents, and we query memory when asking for the list of children.

	ENGINEGUI_API Rect<float> getRect() const;
	ENGINEGUI_API Rect<float> getLocalRect() const;
	ENGINEGUI_API PositioningStrategy* getPositioningStrategy() const;

	template<typename T, typename... TCtorArgs>
	T* setPositioningStrategy(TCtorArgs... ctorArgs)
	{
		static_assert(std::is_base_of_v<PositioningStrategy, T>);
		T* pos = hud->getMemory()->create<T>( std::forward<TCtorArgs...>(ctorArgs)... );
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
	ENGINEGUI_API HUD* getHUD() const;
	ENGINEGUI_API bool isDirty() const;

	ENGINEGUI_API operator glm::mat4() const; //GL interop

private:
	HUD* hud;
	Widget* widget;

	WidgetTransform* parent;
	std::vector<WidgetTransform*> children;
	size_t childIndex;
	depth_t relativeRenderDepth = 0;

	//Cached values
	mutable depth_t renderDepth;
	mutable Rect<float> localRect; //Output of positioning strategy
	mutable Rect<float> rect; //Derived from localRect
	mutable bool refreshing; //Acts as a canary in case PositioningStrategy does something stupid like call a dependent function mid-evaluate
	mutable bool dirty;

	friend class HUD; //Needs to write rect/localrect on root
	friend class LinearLayoutGroupWidget; //FIXME awful workaround

	PositioningStrategy* positioningStrategy;

	void refresh() const;
	void markDirty() const;


	WidgetTransform(const WidgetTransform& cpy) = delete;
	WidgetTransform& operator=(const WidgetTransform& cpy) = delete;
	WidgetTransform(WidgetTransform&& mov) = delete;
	WidgetTransform& operator=(WidgetTransform&& mov) = delete;
};


//A thin wrapper over a sub-transform that limits it to only one element with preset positioning.
//Similar to dependency injection, but for positioning instead of logic.
class WidgetSocket
{
private:
	WidgetTransform* transform; //Owns this
	
public:
	ENGINEGUI_API WidgetSocket(HUD* hud, Widget* owner);
	ENGINEGUI_API ~WidgetSocket();

	ENGINEGUI_API void put(Widget* w);
	ENGINEGUI_API void clear();
	ENGINEGUI_API Widget* get() const;

	ENGINEGUI_API void setRelativeRenderDepth(WidgetTransform::depth_t depth);
	ENGINEGUI_API WidgetTransform::depth_t getRelativeRenderDepth() const;
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
