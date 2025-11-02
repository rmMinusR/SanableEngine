#include "menu/MenuDivider_Linux.hpp"

#include <cassert>

UINT MenuDivider_Linux::native_id() const
{
	return static_cast<UINT>(reinterpret_cast<ULONG_PTR>(this));
}

MenuDivider_Linux::MenuDivider_Linux(HMENU root, MenuContainer* parent, size_t index)
{
	this->parent = parent;
	this->root = root;

	MENUITEMINFOW info;
	info.cbSize = sizeof(info);
	info.fMask = MIIM_FTYPE | MIIM_ID;
	info.fType = MFT_MENUBREAK;
	info.wID = native_id();
	bool ok = InsertMenuItemW(root, index, TRUE, &info);
	assert(ok);
}

MenuDivider_Linux::~MenuDivider_Linux()
{
	bool ok = RemoveMenu(root, native_id(), MF_BYCOMMAND);
	assert(ok);
}
