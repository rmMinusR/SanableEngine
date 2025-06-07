#pragma once

#include <Windows.h>

#include "menu/MenuDivider.hpp"

class MenuDivider_Win32 : public MenuDivider
{
	HMENU root;
	UINT native_id() const;
public:
	MenuDivider_Win32(HMENU root, MenuContainer* parent, size_t index);
	virtual ~MenuDivider_Win32();
};
