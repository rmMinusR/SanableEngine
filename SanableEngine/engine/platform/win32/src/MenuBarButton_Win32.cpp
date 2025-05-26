#include "MenuBarButton_Win32.hpp"

#include "MenuBar_Win32.hpp"

MenuBarButton_Win32::MenuBarButton_Win32(MenuBar_Win32* menubar, std::wstring text) :
	MenuButton(text)
{
	parent = menubar;
}
