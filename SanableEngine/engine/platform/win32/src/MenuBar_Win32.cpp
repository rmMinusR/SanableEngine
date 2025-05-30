#include "menu/MenuBar_Win32.hpp"

#include <SDL_video.h>

#include "Window_Win32.hpp"
#include "menu/MenuButton_Win32.hpp"
#include "menu/MenuDivider_Win32.hpp"
#include "menu/Submenu_Win32.hpp"

MenuBar_Win32::MenuBar_Win32(Window_Win32* window) :
	MenuBar(window),
	window(window)
{
	nativeHandle = CreateMenu();
	MENUINFO options;
	options.cbSize = sizeof(options);
	options.fMask = MIM_STYLE;
	options.dwStyle = MNS_NOTIFYBYPOS;
	SetMenuInfo(nativeHandle, &options);

	SetMenu(window->getNativeHandle(), nativeHandle);
	redraw();
}

void MenuBar_Win32::redraw() const
{
	DrawMenuBar(window->getNativeHandle());
}

MenuBar_Win32::~MenuBar_Win32()
{
	DestroyMenu(nativeHandle);
}

MenuButton* MenuBar_Win32::addButton(std::wstring text, size_t index)
{
	MenuButton_Win32* item = new MenuButton_Win32(nativeHandle, this, index, text);
	registerItem(item, index);
	return item;
}

MenuDivider* MenuBar_Win32::addDivider(size_t index)
{
	return nullptr; // Not allowed. Maybe in future we can support spacers?
}

Submenu* MenuBar_Win32::addSubmenu(std::wstring text, size_t index)
{
	Submenu_Win32* item = new Submenu_Win32(nativeHandle, this, index, text);
	registerItem(item, index);
	return item;
}
