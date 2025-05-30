#pragma once

#include <Windows.h>
#include "menu/Submenu.hpp"

class Submenu_Win32 : public Submenu
{
	HMENU root;
	UINT native_id() const;
	HMENU submenuHandle;

public:
	Submenu_Win32(HMENU root, MenuContainer* parent, size_t index, std::wstring text);
	virtual ~Submenu_Win32();

	virtual MenuButton* addButton(std::wstring text, size_t index = 0);
	virtual MenuDivider* addDivider(size_t index = 0);
	virtual Submenu* addSubmenu(std::wstring text, size_t index = 0);
};
