#pragma once

#include <type_traits>

#include "CallBatcher.inl"
#include "Widget.hpp"
#include "MemoryHeap.hpp"
#include "PoolCallBatcher.hpp"

class Window;
class Application;

class HUD
{
	TypedMemoryPool<WidgetTransform>* transforms;
	PoolCallBatcher<Widget> widgets;
	MemoryHeap memory; //TODO reload safety
	WidgetTransform* root;
	
	Application* application;
	Window* window = nullptr;

	//Concurrency buffers
	std::vector<Widget*> addQueue;
	std::vector<Widget*> removeQueue;
	void applyConcurrencyBuffers();

	ENGINEGUI_API void addWidget_internal(Widget* widget);
	ENGINEGUI_API void destroyWidget_internal(Widget* widget);
public:
	ENGINEGUI_API HUD(Application* application);
	ENGINEGUI_API ~HUD();

	ENGINEGUI_API void init(Window* window);

	ENGINEGUI_API MemoryHeap* getMemory();
	ENGINEGUI_API Window* getWindow() const;
	ENGINEGUI_API Application* getApplication() const;
	
	ENGINEGUI_API void refreshLayout(Rect<float> viewport);
	ENGINEGUI_API void tick();
	ENGINEGUI_API void render(Renderer* renderer);

	ENGINEGUI_API size_t raycast(Vector2f pos, WidgetTransform** hitsOut, size_t hitsOutMaxSz, bool exact = true) const;
	ENGINEGUI_API void raycast(Vector2f pos, const std::function<void(WidgetTransform*)>& visitor, bool exact = true) const;
	ENGINEGUI_API WidgetTransform* raycastClosest(Vector2f pos, bool exact = true) const;

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
	inline void destroyWidget(T* w)
	{
		static_assert(std::is_base_of_v<Widget, T>);
		destroyWidget_internal(w);
		memory.destroy(w);
	}
};
