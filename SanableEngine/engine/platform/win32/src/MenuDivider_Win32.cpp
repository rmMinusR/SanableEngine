#include "menu/MenuDivider_Win32.hpp"

#include <cassert>

UINT MenuDivider_Win32::native_id() const
{
	return static_cast<UINT>(reinterpret_cast<ULONG_PTR>(this));
}

MenuDivider_Win32::MenuDivider_Win32(HMENU root, MenuContainer* parent, size_t index)
{
	this->parent = parent;
	this->root = root;

	MENUITEMINFOW info;
	info.cbSize = sizeof(info);
	info.fMask = MIIM_FTYPE;
	info.fType = MFT_MENUBREAK;
	bool ok = InsertMenuItemW(root, index, TRUE, &info);
	assert(ok);
}

MenuDivider_Win32::~MenuDivider_Win32()
{
	bool ok = RemoveMenu(root, native_id(), MF_BYCOMMAND);
	assert(ok);
}
