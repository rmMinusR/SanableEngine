#include "menu/MenuBar_Linux.hpp"

#include <SDL_video.h>

#include "Window_Linux.hpp"
#include "menu/MenuButton_Linux.hpp"
#include "menu/MenuDivider_Linux.hpp"
#include "menu/Submenu_Linux.hpp"

MenuBar_Linux::MenuBar_Linux(Window_Linux* window) :
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

void MenuBar_Linux::redraw() const
{
	DrawMenuBar(window->getNativeHandle());
}

MenuBar_Linux::~MenuBar_Linux()
{
	MenuContainer::clear();
	DestroyMenu(nativeHandle);
}

MenuButton* MenuBar_Linux::addButton(std::wstring text, size_t index)
{
	MenuButton_Linux* item = new MenuButton_Linux(nativeHandle, this, index, text);
	registerItem(item, index);
	return item;
}

MenuDivider* MenuBar_Linux::addDivider(size_t index)
{
	return nullptr; // Not allowed. Maybe in future we can support spacers?
}

Submenu* MenuBar_Linux::addSubmenu(std::wstring text, size_t index)
{
	Submenu_Linux* item = new Submenu_Linux(nativeHandle, this, index, text);
	registerItem(item, index);
	return item;
}
