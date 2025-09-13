#include "Window_Win32.hpp"

#include <SDL.h>
#include <SDL_syswm.h>
#include "application/Application.hpp"
#include "GLSettings.hpp"
#include "GLContext.hpp"
#include "WindowUserLogic.hpp"
#include "menu/MenuBar_Win32.hpp"
#include "menu/MenuButton.hpp"

Window_Win32::Window_Win32(const WindowSettings& settings, Application* engine, SDL_Window* handle) :
    Window(settings, engine->getSystem(), engine),
    sdlHandle(handle),
    renderer(this, GLContext::create(handle, this))
{
    sdlID = SDL_GetWindowID(handle);

    printf("Window '%s' - OpenGL %s\n", settings.name.c_str(), (char*)glGetString(GL_VERSION));
}

Window_Win32::~Window_Win32()
{
    if (menuBar)
    {
        delete menuBar;
        menuBar = nullptr;
    }
    
    if (sdlHandle)
    {
        SDL_DestroyWindow(sdlHandle);
        sdlHandle = nullptr;
        SDL_QuitSubSystem(SDL_INIT_VIDEO); //Internally refcounted, no checks necessary
    }
}

void Window_Win32::move(int x, int y)
{
    SDL_SetWindowPosition(sdlHandle, x, y);
}

int Window_Win32::getWidth() const
{
    int w;
    SDL_GetWindowSize(sdlHandle, &w, nullptr);
    return w;
}

int Window_Win32::getHeight() const
{
    int h;
    SDL_GetWindowSize(sdlHandle, nullptr, &h);
    return h;
}

Vector2<int> Window_Win32::getSize() const
{
    Vector2<int> size;
    SDL_GetWindowSize(sdlHandle, &size.x, &size.y);
    return size;
}

void Window_Win32::setSize(int w, int h)
{
    assert(w > 0 && h > 0);
    SDL_SetWindowSize(sdlHandle, w, h);
    onResized();
}

void Window_Win32::handleEvent(const SDL_Event& ev)
{
    if(ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        Vector2<int> newSize(ev.window.data1, ev.window.data2);
    }
    Window::handleEvent(ev);
}

void Window_Win32::setUserResizable(bool val)
{
    SDL_SetWindowResizable(sdlHandle, val ? SDL_TRUE : SDL_FALSE);
}

bool Window_Win32::wasCloseRequested() const
{
    return closeRequested;
}

void Window_Win32::setActiveDrawTarget() const
{
    int err = SDL_GL_MakeCurrent(sdlHandle, renderer.sdlHandle());
    assert(!err);
    assert(SDL_GL_GetCurrentWindow() == sdlHandle);
    assert(SDL_GL_GetCurrentContext() == renderer.sdlHandle());
}

void Window_Win32::draw() const
{
    //Reset to default state
    setActiveDrawTarget();
    int width, height;
    SDL_GetWindowSize(sdlHandle, &width, &height);
    glViewport(0, 0, width, height); // FIXME move into OpenGlRenderer

    //Delegate draw
    Window::draw();

    //Swap back buffer
    SDL_GL_SwapWindow(sdlHandle);
}

MenuBar* Window_Win32::getMenuBar(bool create)
{
    if (create && !menuBar)
    {
        menuBar = new MenuBar_Win32(this);
    }
    return menuBar;
}

const MenuBar* Window_Win32::getMenuBar() const
{
    return menuBar;
}

SDL_Window* Window_Win32::getSdlHandle() const
{
    return sdlHandle;
}

HWND Window_Win32::getNativeHandle()
{
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(sdlHandle, &wmInfo);
    return wmInfo.info.win.window;
}

void Window_Win32::handleNativeEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_MENUCOMMAND)
    {
        // wParam = index, lParam = owning menu handle
        MENUITEMINFOW info;
        info.cbSize = sizeof(info);
        info.fMask = MIIM_DATA;

        bool ok = GetMenuItemInfoW((HMENU)lParam, wParam, TRUE, &info);
        assert(ok);

        MenuItem* clicked = (MenuItem*)info.dwItemData;
        if (MenuButton* btn = dynamic_cast<MenuButton*>(clicked))
        {
            btn->onClick();
        }
    }
}