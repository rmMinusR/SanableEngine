#include "MenuItem.hpp"

#include <cassert>

#include "MenuButton.hpp"
#include "MenuDivider.hpp"

MenuItem::MenuItem()
{
}

MenuItem::~MenuItem()
{
	while(!children.empty()) delete children.back(); // Child erases self from parent
	
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

MenuItem* MenuItem::getParent()
{
	return parent;
}

const MenuItem* MenuItem::getParent() const
{
	return parent;
}

size_t MenuItem::getNumChildren() const
{
	return children.size();
}

MenuItem* MenuItem::getChild(size_t which)
{
	return children[which];
}

const MenuItem* MenuItem::getChild(size_t which) const
{
	return children[which];
}

void MenuItem::registerItem(MenuItem* item, size_t index)
{
	assert(!item->parent);
	item->parent = this;
	children.insert(children.begin()+index, item);
}
