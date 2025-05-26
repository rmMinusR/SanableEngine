#include "MenuButton_Win32.hpp"

#include "MenuDivider_Win32.hpp"

MenuButton_Win32::MenuButton_Win32(MenuItem* parent, size_t index, std::wstring text) :
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
}

MenuButton* MenuButton_Win32::addButton(std::wstring text, size_t index)
{
	MenuButton_Win32* item = new MenuButton_Win32(this, index, text);
	registerItem(item, index);
	return item;
}

MenuDivider* MenuButton_Win32::addDivider(size_t index)
{
	MenuDivider_Win32* item = new MenuDivider_Win32(this, index);
	registerItem(item, index);
	return item;
}

void* MenuButton_Win32::getNativeHandle()
{
	return nativeHandle;
}

const void* MenuButton_Win32::getNativeHandle() const
{
	return nativeHandle;
}
