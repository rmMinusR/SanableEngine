#pragma once

#include "menu/MenuItem.hpp"

class Window;

class MenuBar : public MenuContainer
{
protected:
	//friend class Window;
	MenuBar(Window* window);
public:
	virtual ~MenuBar();
};
