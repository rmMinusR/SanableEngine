#include "Window_Win32.hpp"

#include <SDL.h>
#include "application/Application.hpp"
#include "GLContext.hpp"
#include "GLSettings.hpp"

Window_Win32::Window_Win32(const WindowSettings& settings, const GLSettings& glSettings, Application* engine) :
    Window(settings, engine->getSystem(), engine)
{
    SDL_InitSubSystem(SDL_INIT_VIDEO); //Internally refcounted, no checks necessary

    glSettings.apply();

    handle = SDL_CreateWindow(
        settings.name.c_str(),
        settings.position.has_value() ? settings.position->x : SDL_WINDOWPOS_UNDEFINED,
        settings.position.has_value() ? settings.position->y : SDL_WINDOWPOS_UNDEFINED,
        settings.size.x,
        settings.size.y,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );
    context = GLContext::create(handle, this);

    _interface = OpenGlRenderer(this, context);
    sdlID = SDL_GetWindowID(handle);

    printf("Window '%s' - OpenGL %s\n", settings.name.c_str(), (char*)glGetString(GL_VERSION));
}

Window_Win32::~Window_Win32()
{
    std::vector<Window*>& windows = engine->windows;
    windows.erase(std::find(windows.begin(), windows.end(), this));

    if (_interface)
    {
        delete _interface;
        _interface = nullptr;
    }

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

bool Window_Win32::isFocused() const
{
    return currentFocus == this;
}

bool Window_Win32::wasCloseRequested() const
{
    return closeRequested;
}

void Window_Win32::setActiveDrawTarget()
{
    SDL_GL_MakeCurrent(handle, context);
    assert(SDL_GL_GetCurrentWindow() == handle);
    assert(SDL_GL_GetCurrentContext() == context);
}

void Window_Win32::draw() const
{
    //Reset to default state
    setActiveDrawTarget(this);
    int width, height;
    SDL_GetWindowSize(handle, &width, &height);
    glViewport(0, 0, width, height); // FIXME move into OpenGlRenderer

    //Delegate draw
    if (renderPipeline) renderPipeline->render({ Vector2f(0,0), Vector2f(width, height) });

    //Swap back buffer
    SDL_GL_SwapWindow(handle);
}

void Window_Win32::handleEvent(SDL_Event& ev)
{
    if (ev.type == SDL_EventType::SDL_WINDOWEVENT)
    {
        switch (ev.window.event)
        {
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            Window::currentFocus = (Window*)this;
            break;

        case SDL_WINDOWEVENT_FOCUS_LOST:
            if (Window::currentFocus == this) Window::currentFocus = nullptr;
            break;

        case SDL_WINDOWEVENT_CLOSE:
            closeRequested = true;
            break;
        }

        //Drawing stops when grabbing. This might fix? Or might not. Who knows.
        //if (ev.window.event == SDL_WINDOWEVENT_MOVED || ev.window.event == SDL_WINDOWEVENT_RESIZED) draw();
    }

    if (inputProcessor) inputProcessor->handleEvent(ev);
}
