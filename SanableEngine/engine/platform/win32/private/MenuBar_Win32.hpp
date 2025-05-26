#pragma once

#include <Windows.h>
#include "MenuItem.hpp"

class Window_Win32;

class MenuBar_Win32 : public MenuItem
{
	Window_Win32* window;
	HMENU nativeHandle;
public:
	MenuBar_Win32(Window_Win32* window);
	virtual ~MenuBar_Win32();

	virtual void* getNativeHandle() override;
	virtual const void* getNativeHandle() const override;

	virtual MenuButton* addButton(std::wstring text, size_t index = 0) override;
	virtual MenuDivider* addDivider(size_t index = 0) override;
};
