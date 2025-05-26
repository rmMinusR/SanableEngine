#include "menu/MenuContainer.hpp"

#include <cassert>

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
	assert(!item->parent);
	item->parent = this;
	children.insert(children.begin() + index, item);
}
