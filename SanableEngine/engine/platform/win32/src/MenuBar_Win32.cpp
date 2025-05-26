#include "MenuBar_Win32.hpp"

#include <SDL_video.h>

#include "Window_Win32.hpp"
#include "MenuButton_Win32.hpp"
#include "MenuDivider_Win32.hpp"

MenuBar_Win32::MenuBar_Win32(Window_Win32* window) :
	window(window)
{
	nativeHandle = CreateMenu();
	SetMenu(window->getNativeHandle(), nativeHandle);
	DrawMenuBar(window->getNativeHandle());

	// TODO register items for WM_COMMAND?
}

MenuBar_Win32::~MenuBar_Win32()
{
	DestroyMenu(nativeHandle);
}

MenuButton* MenuBar_Win32::addButton(std::wstring text, size_t index)
{
	MenuButton_Win32* item = new MenuButton_Win32(this, index, text);
	registerItem(item, index);
	return item;
}

MenuDivider* MenuBar_Win32::addDivider(size_t index)
{
	return nullptr; // Not allowed. Maybe in future we can support spacers?
}

void* MenuBar_Win32::getNativeHandle()
{
	return nativeHandle;
}

const void* MenuBar_Win32::getNativeHandle() const
{
	return nativeHandle;
}
