#include "MenuDivider.hpp"

MenuDivider::MenuDivider()
{
}

size_t MenuDivider::getNumChildren() const
{
	return 0;
}

MenuItem* MenuDivider::getChild(size_t which)
{
	return nullptr;
}

const MenuItem* MenuDivider::getChild(size_t which) const
{
	return nullptr;
}

MenuButton* MenuDivider::addButton(std::wstring text, size_t index)
{
	return nullptr;
}

MenuDivider* MenuDivider::addDivider(size_t index)
{
	return nullptr;
}
