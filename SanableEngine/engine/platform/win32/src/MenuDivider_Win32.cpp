#include "menu/MenuDivider_Win32.hpp"

MenuDivider_Win32::MenuDivider_Win32(MenuContainer* parent, size_t index)
{
	this->parent = parent;

	nativeHandle = CreateMenu();

	MENUITEMINFOW info;
	info.cbSize = sizeof(MENUITEMINFOW);
	info.fMask = MIIM_FTYPE;
	info.fType = MFT_MENUBREAK;
	InsertMenuItemW(nativeHandle, index, TRUE, &info);
}

MenuDivider_Win32::~MenuDivider_Win32()
{
	DestroyMenu(nativeHandle);
}
