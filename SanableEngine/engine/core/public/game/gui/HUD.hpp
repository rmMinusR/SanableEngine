#pragma once

#include <type_traits>

#include "CallBatcher.inl"
#include "Widget.hpp"
#include "MemoryManager.hpp"

class HUD
{
private:
	CallBatcher<Widget> widgets;
	MemoryManager memory; //TODO reload safety
	
public:
	ENGINEGUI_API HUD();
	ENGINEGUI_API ~HUD();

	ENGINEGUI_API MemoryManager* getMemory();

	ENGINEGUI_API void render(Renderer* renderer);

	template<typename T, typename... TCtorArgs>
	inline T* addWidget(TCtorArgs... ctorArgs)
	{
		static_assert(std::is_base_of_v<Widget, T>);
		T* widget = memory.create<T, TCtorArgs...>(ctorArgs...);
		widget->hud = this;
		widgets.add(widget);
		return widget;
	}

	template<typename T>
	inline void removeWidget(T* w)
	{
		static_assert(std::is_base_of_v<Widget, T>);
		widgets.remove(w);
		memory.destroy(w);
	}
};
