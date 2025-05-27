#pragma once

#include <Windows.h>

#include "menu/MenuBar.hpp"

class Window_Win32;

class MenuBar_Win32 : public MenuBar
{
	Window_Win32* window;
	HMENU nativeHandle;

	friend class Window_Win32;
	MenuBar_Win32(Window_Win32* window);
public:
	virtual ~MenuBar_Win32();

	virtual MenuButton* addButton(std::wstring text, size_t index = 0) override;
	virtual MenuDivider* addDivider(size_t index = 0) override;
	virtual Submenu* addSubmenu(std::wstring text, size_t index = 0) override;
};
