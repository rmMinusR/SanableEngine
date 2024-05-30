#pragma once

#include <type_traits>

#include "CallBatcher.inl"
#include "Widget.hpp"
#include "MemoryManager.hpp"
#include "PoolCallBatcher.hpp"

class HUD
{
private:
	TypedMemoryPool<WidgetTransform>* transforms;
	PoolCallBatcher<Widget> widgets;
	MemoryManager memory; //TODO reload safety
	WidgetTransform* root;
	
	//Concurrency buffers
	std::vector<Widget*> addQueue;
	std::vector<Widget*> removeQueue;
	void applyConcurrencyBuffers();

	ENGINEGUI_API void addWidget_internal(Widget* widget);
	ENGINEGUI_API void removeWidget_internal(Widget* widget);
public:
	ENGINEGUI_API HUD();
	ENGINEGUI_API ~HUD();

	ENGINEGUI_API MemoryManager* getMemory();
	
	ENGINEGUI_API void refreshLayout(Rect<float> viewport);
	ENGINEGUI_API void tick();
	ENGINEGUI_API void render(Renderer* renderer);

	ENGINEGUI_API void raycast(Vector2f pos, const std::function<void(Widget*)>& visitor, bool exact = true) const;
	ENGINEGUI_API Widget* raycastClosest(Vector2f pos, bool exact = true) const;

	ENGINEGUI_API WidgetTransform const* getRootTransform() const;
	ENGINEGUI_API WidgetTransform* getRootTransform();
	
	template<typename T, typename... TCtorArgs>
	inline T* addWidget(TCtorArgs... ctorArgs)
	{
		static_assert(std::is_base_of_v<Widget, T>);
		T* widget = memory.create<T, HUD*, TCtorArgs...>(this, ctorArgs...);
		addWidget_internal(widget);
		return widget;
	}

	template<typename T>
	inline void removeWidget(T* w)
	{
		static_assert(std::is_base_of_v<Widget, T>);
		removeWidget_internal(w);
		memory.destroy(w);
	}

};
