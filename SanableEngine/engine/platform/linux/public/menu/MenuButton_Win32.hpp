#pragma once

#include <Windows.h>

#include "menu/MenuButton.hpp"

class MenuButton_Win32 : public MenuButton
{
	HMENU root;
	UINT native_id() const;
public:
	MenuButton_Win32(HMENU root, MenuContainer* parent, size_t index, std::wstring text);
	virtual ~MenuButton_Win32();

	virtual void setEnabled(bool enable) override;
	virtual bool isEnabled() const override;
};
