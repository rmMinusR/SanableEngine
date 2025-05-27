#include "menu/MenuButton_Win32.hpp"

#include "menu/MenuDivider_Win32.hpp"

MenuButton_Win32::MenuButton_Win32(MenuContainer* parent, size_t index, std::wstring text) :
	MenuButton(text)
{
	this->parent = parent;

	nativeHandle = CreateMenu();

	MENUITEMINFOW info;
	info.cbSize = sizeof(MENUITEMINFOW);
	info.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STRING;
	info.dwItemData = reinterpret_cast<ULONG_PTR>(this);
	info.fType = MFT_STRING;
	info.dwTypeData = this->text.data();
	info.cch = this->text.size();
	InsertMenuItemW(nativeHandle, index, TRUE, &info);
}

MenuButton_Win32::~MenuButton_Win32()
{
	DestroyMenu(nativeHandle);
}
