#include "menu/MenuItem.hpp"

#include <cassert>

#include "menu/MenuButton.hpp"
#include "menu/MenuDivider.hpp"

MenuItem::MenuItem()
{
}

MenuItem::~MenuItem()
{
	// Erase self from parent
	if (parent)
	{
		auto it = std::find(parent->children.begin(), parent->children.end(), this);
		parent->children.erase(it);
	}
}

size_t MenuItem::getIndexInParent() const
{
	auto it = std::find(parent->children.begin(), parent->children.end(), this);
	return std::distance(parent->children.begin(), it);
}

MenuContainer* MenuItem::getParent()
{
	return parent;
}

const MenuContainer* MenuItem::getParent() const
{
	return parent;
}
