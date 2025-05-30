#include "menu/Submenu_Win32.hpp"

#include <cassert>

#include "menu/MenuButton_Win32.hpp"
#include "menu/MenuDivider_Win32.hpp"
#include "menu/Submenu_Win32.hpp"

UINT Submenu_Win32::native_id() const
{
	return static_cast<UINT>(reinterpret_cast<ULONG_PTR>(this));
}

Submenu_Win32::Submenu_Win32(HMENU root, MenuContainer* parent, size_t index, std::wstring text) :
	Submenu(text)
{
	this->parent = parent;
	this->root = root;

	submenuHandle = CreatePopupMenu();
	MENUINFO submenuOptions;
	submenuOptions.cbSize = sizeof(submenuOptions);
	submenuOptions.fMask = MIM_STYLE;
	submenuOptions.dwStyle = MNS_NOTIFYBYPOS;
	SetMenuInfo(submenuHandle, &submenuOptions);

	MENUITEMINFOW info;
	info.cbSize = sizeof(info);
	info.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID | MIIM_SUBMENU;
	info.fType = MFT_STRING;
	info.dwTypeData = this->text.data();
	info.cch = this->text.size();
	info.wID = native_id();
	info.hSubMenu = submenuHandle;
	InsertMenuItemW(root, index, TRUE, &info);
}

Submenu_Win32::~Submenu_Win32()
{
	while (!children.empty())
	{
		delete children.back();
		// MenuItem erases itself from parent container
	}

	bool ok = RemoveMenu(root, native_id(), MF_BYCOMMAND); // Implies DestroyMenu(submenuHandle)
	assert(ok);
}

MenuButton* Submenu_Win32::addButton(std::wstring text, size_t index)
{
	MenuButton_Win32* item = new MenuButton_Win32(submenuHandle, this, index, text);
	registerItem(item, index);
	return item;
}

MenuDivider* Submenu_Win32::addDivider(size_t index)
{
	MenuDivider_Win32* item = new MenuDivider_Win32(submenuHandle, this, index);
	registerItem(item, index);
	return item;
}

Submenu* Submenu_Win32::addSubmenu(std::wstring text, size_t index)
{
	Submenu_Win32* item = new Submenu_Win32(submenuHandle, this, index, text);
	registerItem(item, index);
	return item;
}
