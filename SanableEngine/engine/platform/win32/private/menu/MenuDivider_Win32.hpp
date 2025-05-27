#pragma once

#include <Windows.h>

#include "menu/MenuDivider.hpp"

class MenuDivider_Win32 : public MenuDivider
{
	HMENU nativeHandle;
public:
	MenuDivider_Win32(MenuContainer* parent, size_t index);
	virtual ~MenuDivider_Win32();
};
