#include "Window_Win32.hpp"

#include <SDL.h>
#include "WindowRenderPipeline.hpp"
#include "application/Application.hpp"
#include "GLContext.hpp"
#include "GLSettings.hpp"

Window_Win32::Window_Win32(const WindowSettings& settings, Application* engine, SDL_Window* handle) :
    Window(settings, engine->getSystem(), engine),
    renderer(this, handle)
{
    sdlID = SDL_GetWindowID(handle);

    printf("Window '%s' - OpenGL %s\n", settings.name.c_str(), (char*)glGetString(GL_VERSION));
}

Window_Win32::~Window_Win32()
{
    if (context)
    {
        GLContext::release(context, this);
        context = nullptr;
    }

    if (handle)
    {
        SDL_DestroyWindow(handle);
        handle = nullptr;
        SDL_QuitSubSystem(SDL_INIT_VIDEO); //Internally refcounted, no checks necessary
    }
}

void Window_Win32::move(int x, int y)
{
    SDL_SetWindowPosition(handle, x, y);
}

int Window_Win32::getWidth() const
{
    int w;
    SDL_GetWindowSize(handle, &w, nullptr);
    return w;
}

int Window_Win32::getHeight() const
{
    int h;
    SDL_GetWindowSize(handle, nullptr, &h);
    return h;
}

Vector2<int> Window_Win32::getSize() const
{
    Vector2<int> size;
    SDL_GetWindowSize(handle, &size.x, &size.y);
    return size;
}

bool Window_Win32::wasCloseRequested() const
{
    return closeRequested;
}

void Window_Win32::setActiveDrawTarget() const
{
    SDL_GL_MakeCurrent(handle, context);
    assert(SDL_GL_GetCurrentWindow() == handle);
    assert(SDL_GL_GetCurrentContext() == context);
}

void Window_Win32::draw() const
{
    //Reset to default state
    setActiveDrawTarget();
    int width, height;
    SDL_GetWindowSize(handle, &width, &height);
    glViewport(0, 0, width, height); // FIXME move into OpenGlRenderer

    //Delegate draw
    Window::draw();

    //Swap back buffer
    SDL_GL_SwapWindow(handle);
}
