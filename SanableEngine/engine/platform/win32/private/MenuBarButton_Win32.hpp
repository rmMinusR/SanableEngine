#pragma once

#include <string>
#include <Windows.h>

#include "MenuButton.hpp"

class MenuBar_Win32;

class MenuBarButton_Win32 : public MenuButton
{
	HMENU nativeHandle;

public:
	MenuBarButton_Win32(MenuBar_Win32* menubar, std::wstring text);

	virtual MenuButton* addButton(std::wstring text, size_t index = 0) override;
	virtual MenuDivider* addDivider(size_t index = 0) override;

	virtual void* getNativeHandle() override;
	virtual const void* getNativeHandle() const override;

	virtual void setEnabled(bool) override;
	virtual bool isEnabled() const override;
	virtual void setCallback(std::function<void()> callback) override;
};
