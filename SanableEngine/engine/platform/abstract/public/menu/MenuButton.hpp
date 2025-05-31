#pragma once

#include <functional>

#include "MenuItem.hpp"

class MenuButton : public MenuItem
{
protected:
	std::wstring text;
	std::function<void()> callback;

	MenuButton(std::wstring text);

public:
	virtual ~MenuButton();

	virtual void setEnabled(bool) = 0;
	virtual bool isEnabled() const = 0;

	virtual void setCallback(std::function<void()> callback);
	virtual void onClick() const;
};
