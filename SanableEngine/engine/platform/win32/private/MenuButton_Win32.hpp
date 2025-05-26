#pragma once

#include <Windows.h>

#include "MenuButton.hpp"

class MenuButton_Win32 : public MenuButton
{
	HMENU nativeHandle;
public:
	MenuButton_Win32(MenuItem* parent, size_t index, std::wstring text);
	virtual ~MenuButton_Win32();

	virtual MenuButton* addButton(std::wstring text, size_t index = 0) override;
	virtual MenuDivider* addDivider(size_t index = 0) override;

	virtual void* getNativeHandle() override;
	virtual const void* getNativeHandle() const override;

	virtual void setEnabled(bool) override;
	virtual bool isEnabled() const override;
	virtual void setCallback(std::function<void()> callback) override;
};
