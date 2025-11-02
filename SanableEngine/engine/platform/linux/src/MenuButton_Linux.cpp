#include "menu/MenuButton_Linux.hpp"

#include <cassert>
#include "menu/MenuDivider_Linux.hpp"

UINT MenuButton_Linux::native_id() const
{
	return static_cast<UINT>(reinterpret_cast<ULONG_PTR>(this));
}

MenuButton_Linux::MenuButton_Linux(HMENU root, MenuContainer* parent, size_t index, std::wstring text) :
	MenuButton(text)
{
	this->parent = parent;
	this->root = root;

	MENUITEMINFOW info;
	info.cbSize = sizeof(info);
	info.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STRING | MIIM_ID;
	info.dwItemData = reinterpret_cast<ULONG_PTR>(static_cast<MenuItem*>(this));
	info.fType = MFT_STRING;
	info.dwTypeData = this->text.data();
	info.cch = this->text.size();
	info.wID = native_id();
	bool ok = InsertMenuItemW(root, index, TRUE, &info);
	assert(ok);
}

MenuButton_Linux::~MenuButton_Linux()
{
	bool ok = RemoveMenu(root, native_id(), MF_BYCOMMAND);
	assert(ok);
}

void MenuButton_Linux::setEnabled(bool enable)
{
	EnableMenuItem(root, native_id(), MF_BYCOMMAND | (enable ? MF_ENABLED : MF_DISABLED));
}

bool MenuButton_Linux::isEnabled() const
{
	MENUITEMINFOW info;
	info.cbSize = sizeof(info);
	info.fMask = MIIM_STATE;
	GetMenuItemInfoW(root, native_id(), FALSE, &info);

	return !(info.fState & MFS_DISABLED);
}
