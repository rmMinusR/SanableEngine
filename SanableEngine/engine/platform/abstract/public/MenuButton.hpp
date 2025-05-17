#pragma once

#include <functional>

#include "MenuItem.hpp"

class MenuButton : public MenuItem
{
protected:
	std::wstring text;
	std::function<void()> callback;

	MenuButton(MenuItemFactory* itemFactory, std::wstring text);

public:
	virtual ~MenuButton() = default;

	virtual void setEnabled(bool) = 0;
	virtual bool isEnabled() = 0;

	virtual void setCallback(std::function<void()> callback) = 0;
};
