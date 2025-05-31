#include "Window_Win32.hpp"

#include <SDL.h>
#include "application/Application.hpp"
#include "GLSettings.hpp"
#include "GLContext.hpp"

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

bool Window_Win32::wasCloseRequested() const
{
    return closeRequested;
}

void Window_Win32::setActiveDrawTarget() const
{
    SDL_GL_MakeCurrent(sdlHandle, renderer.sdlHandle());
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
