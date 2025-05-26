#include "MenuDivider_Win32.hpp"

MenuDivider_Win32::MenuDivider_Win32(MenuItem* parent, size_t index)
{
	this->parent = parent;

	MENUITEMINFOW info;
	info.cbSize = sizeof(MENUITEMINFOW);
	info.fMask = MIIM_FTYPE;
	info.fType = MFT_MENUBREAK;
	InsertMenuItemW(nativeHandle, index, TRUE, &info);
}

MenuDivider_Win32::~MenuDivider_Win32()
{
}

MenuButton* MenuDivider_Win32::addButton(std::wstring text, size_t index)
{
	return nullptr; // Not allowed
}

MenuDivider* MenuDivider_Win32::addDivider(size_t index)
{
	return nullptr; // Not allowed
}

void* MenuDivider_Win32::getNativeHandle()
{
	return nativeHandle;
}

const void* MenuDivider_Win32::getNativeHandle() const
{
	return nativeHandle;
}
