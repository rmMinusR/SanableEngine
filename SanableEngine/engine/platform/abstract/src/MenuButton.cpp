#include "menu/MenuButton.hpp"

MenuButton::MenuButton(std::wstring text) :
	text(text)
{
}

MenuButton::~MenuButton()
{
}

void MenuButton::setCallback(std::function<void()> callback)
{
	this->callback = callback;
}

void MenuButton::onClick() const
{
	callback();
}
