#pragma once

#include <Windows.h>

#include "MenuDivider.hpp"

class MenuDivider_Win32 : public MenuDivider
{
	HMENU nativeHandle;
public:
	MenuDivider_Win32(MenuItem* parent, size_t index);
	virtual ~MenuDivider_Win32();

	virtual MenuButton* addButton(std::wstring text, size_t index = 0) override;
	virtual MenuDivider* addDivider(size_t index = 0) override;

	virtual void* getNativeHandle() override;
	virtual const void* getNativeHandle() const override;
};
