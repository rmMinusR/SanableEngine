#include "menu/Submenu_Win32.hpp"

#include "menu/MenuButton_Win32.hpp"
#include "menu/MenuDivider_Win32.hpp"
#include "menu/Submenu_Win32.hpp"

Submenu_Win32::Submenu_Win32(MenuContainer* parent, size_t index, std::wstring text) :
	Submenu(text)
{
	this->parent = parent;

	nativeHandle = CreateMenu();

	MENUITEMINFOW info;
	info.cbSize = sizeof(MENUITEMINFOW);
	info.fMask = MIIM_FTYPE | MIIM_STRING;
	info.fType = MFT_STRING | MF_POPUP;
	info.dwTypeData = this->text.data();
	info.cch = this->text.size();
	InsertMenuItemW(nativeHandle, index, TRUE, &info);
}

Submenu_Win32::~Submenu_Win32()
{
	DestroyMenu(nativeHandle);
}

MenuButton* Submenu_Win32::addButton(std::wstring text, size_t index)
{
	MenuButton_Win32* item = new MenuButton_Win32(this, index, text);
	registerItem(item, index);
	return item;
}

MenuDivider* Submenu_Win32::addDivider(size_t index)
{
	MenuDivider_Win32* item = new MenuDivider_Win32(this, index);
	registerItem(item, index);
	return item;
}

Submenu* Submenu_Win32::addSubmenu(std::wstring text, size_t index)
{
	Submenu_Win32* item = new Submenu_Win32(this, index, text);
	registerItem(item, index);
	return item;
}
