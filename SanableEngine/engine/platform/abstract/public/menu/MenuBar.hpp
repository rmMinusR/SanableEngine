#pragma once

#include "menu/MenuItem.hpp"

class MenuBar : public MenuContainer
{
protected:
	//friend class Window;
	MenuBar(Window* window);
public:
	virtual ~MenuBar();
};
