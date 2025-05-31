#include "menu/MenuContainer.hpp"

#include <cassert>

#include "menu/MenuItem.hpp"

MenuContainer::MenuContainer()
{
}

MenuContainer::~MenuContainer()
{
	while (!children.empty())
	{
		delete children.back(); // Child erases self from parent
	}
}

size_t MenuContainer::getNumChildren() const
{
	return children.size();
}

MenuItem* MenuContainer::getChild(size_t which)
{
	return children[which];
}

const MenuItem* MenuContainer::getChild(size_t which) const
{
	return children[which];
}

void MenuContainer::registerItem(MenuItem* item, size_t index)
{
	assert( item->parent == this && std::find(children.begin(), children.end(), item) == children.end() );
	children.insert(children.begin() + index, item);
}

void MenuContainer::clear()
{
	for (int i = children.size(); i > 0; --i)
	{
		delete children[i-1];
		// MenuItem erases itself from parent container
	}
	assert(children.empty());
}
