#pragma once

#include "CallBatcher.inl"
#include "Widget.hpp"

class HUD
{
private:
	CallBatcher<Widget> widgets;
	
public:
	ENGINEGUI_API HUD();
	ENGINEGUI_API ~HUD();

	ENGINEGUI_API void render(Renderer* renderer);
};
