#include "Window_Linux.hpp"

#include <SDL.h>
#include <SDL_syswm.h>
#include "application/Application.hpp"
#include "GLSettings.hpp"
#include "GLContext.hpp"
#include "menu/MenuBar_Linux.hpp"
#include "menu/MenuButton.hpp"

Window_Linux::Window_Linux(const WindowSettings& settings, Application* engine, SDL_Window* handle) :
    Window(settings, engine->getSystem(), engine),
    sdlHandle(handle),
    renderer(this, GLContext::create(handle, this))
{
    sdlID = SDL_GetWindowID(handle);

    printf("Window '%s' - OpenGL %s\n", settings.name.c_str(), (char*)glGetString(GL_VERSION));
}

Window_Linux::~Window_Linux()
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

void Window_Linux::move(int x, int y)
{
    SDL_SetWindowPosition(sdlHandle, x, y);
}

int Window_Linux::getWidth() const
{
    int w;
    SDL_GetWindowSize(sdlHandle, &w, nullptr);
    return w;
}

int Window_Linux::getHeight() const
{
    int h;
    SDL_GetWindowSize(sdlHandle, nullptr, &h);
    return h;
}

Vector2<int> Window_Linux::getSize() const
{
    Vector2<int> size;
    SDL_GetWindowSize(sdlHandle, &size.x, &size.y);
    return size;
}

bool Window_Linux::wasCloseRequested() const
{
    return closeRequested;
}

void Window_Linux::setActiveDrawTarget() const
{
    SDL_GL_MakeCurrent(sdlHandle, renderer.sdlHandle());
    assert(SDL_GL_GetCurrentWindow() == sdlHandle);
    assert(SDL_GL_GetCurrentContext() == renderer.sdlHandle());
}

void Window_Linux::draw() const
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

MenuBar* Window_Linux::getMenuBar(bool create)
{
    if (create && !menuBar)
    {
        menuBar = new MenuBar_Linux(this);
    }
    return menuBar;
}

const MenuBar* Window_Linux::getMenuBar() const
{
    return menuBar;
}

HWND Window_Linux::getNativeHandle()
{
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(sdlHandle, &wmInfo);
    return wmInfo.info.win.window;
}

void Window_Linux::handleNativeEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
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