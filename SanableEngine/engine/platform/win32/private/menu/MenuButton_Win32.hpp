#pragma once

#include <Windows.h>

#include "menu/MenuButton.hpp"

class MenuButton_Win32 : public MenuButton
{
	HMENU nativeHandle;
public:
	MenuButton_Win32(MenuContainer* parent, size_t index, std::wstring text);
	virtual ~MenuButton_Win32();

	virtual void setEnabled(bool) override;
	virtual bool isEnabled() const override;
	virtual void setCallback(std::function<void()> callback) override;
};
