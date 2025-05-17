#include "MenuButton.hpp"

MenuButton::MenuButton(MenuItemFactory* itemFactory, std::wstring text) :
	MenuItem(itemFactory),
	text(text)
{
}
