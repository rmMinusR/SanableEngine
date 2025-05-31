#pragma once

#include <functional>

#include "MenuItem.hpp"

class MenuDivider : public MenuItem
{
protected:
	MenuDivider();

public:
	virtual ~MenuDivider();
};
