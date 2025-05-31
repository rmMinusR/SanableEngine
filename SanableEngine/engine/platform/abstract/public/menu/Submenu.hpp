#pragma once

#include "MenuItem.hpp"
#include "MenuContainer.hpp"

class Submenu : public MenuItem, public MenuContainer
{
protected:
	std::wstring text;

	Submenu(std::wstring text);

public:
	virtual ~Submenu();
};
