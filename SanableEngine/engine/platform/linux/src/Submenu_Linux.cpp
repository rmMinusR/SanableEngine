#include "menu/Submenu_Linux.hpp"

#include <cassert>

#include "menu/MenuButton_Linux.hpp"
#include "menu/MenuDivider_Linux.hpp"
#include "menu/Submenu_Linux.hpp"

UINT Submenu_Linux::native_id() const
{
	return static_cast<UINT>(reinterpret_cast<ULONG_PTR>(this));
}

Submenu_Linux::Submenu_Linux(HMENU root, MenuContainer* parent, size_t index, std::wstring text) :
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

Submenu_Linux::~Submenu_Linux()
{
	MenuContainer::clear();

	bool ok = RemoveMenu(root, native_id(), MF_BYCOMMAND); // Implies DestroyMenu(submenuHandle)
	assert(ok);
}

MenuButton* Submenu_Linux::addButton(std::wstring text, size_t index)
{
	MenuButton_Linux* item = new MenuButton_Linux(submenuHandle, this, index, text);
	registerItem(item, index);
	return item;
}

MenuDivider* Submenu_Linux::addDivider(size_t index)
{
	MenuDivider_Linux* item = new MenuDivider_Linux(submenuHandle, this, index);
	registerItem(item, index);
	return item;
}

Submenu* Submenu_Linux::addSubmenu(std::wstring text, size_t index)
{
	Submenu_Linux* item = new Submenu_Linux(submenuHandle, this, index, text);
	registerItem(item, index);
	return item;
}
