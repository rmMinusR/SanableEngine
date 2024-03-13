#include "application/Window.hpp"

#include <SDL.h>
#include <GL/glew.h>
#include "GLContext.hpp"
#include "application/WindowRenderPipeline.hpp"
#include "application/Application.hpp"
#include "GLSettings.hpp"

Window* Window::currentFocus = nullptr;

Window::Window(const std::string& name, int width, int height, const GLSettings& glSettings, Application* engine, std::unique_ptr<WindowRenderPipeline>&& renderPipeline, std::unique_ptr<WindowInputProcessor>&& inputProcessor) :
    renderPipeline(std::move(renderPipeline)),
    inputProcessor(std::move(inputProcessor)),
    engine(engine),
    closeRequested(false)
{
    SDL_InitSubSystem(SDL_INIT_VIDEO); //Internally refcounted, no checks necessary

    glSettings.apply();

    handle = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    context = GLContext::create(handle, this);
    
    _interface = Renderer(this, context);
    sdlID = SDL_GetWindowID(handle);
    
    printf("Window '%s' - OpenGL %s\n", name.c_str(), (char*)glGetString(GL_VERSION));
}

Window::~Window()
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

void Window::move(int x, int y)
{
    SDL_SetWindowPosition(handle, x, y);
}

int Window::getWidth() const
{
    int w;
    SDL_GetWindowSize(handle, &w, nullptr);
    return w;
}

int Window::getHeight() const
{
    int h;
    SDL_GetWindowSize(handle, nullptr, &h);
    return h;
}

bool Window::isFocused() const
{
    return currentFocus == this;
}

bool Window::wasCloseRequested() const
{
    return closeRequested;
}

void Window::draw() const
{
    //Reset to default state
    setActiveDrawTarget(this);
    int width, height;
    SDL_GetWindowSize(handle, &width, &height);
    glViewport(0, 0, width, height);

    //Delegate draw
    if (renderPipeline) renderPipeline->render({ Vector2f(0,0), Vector2f(width, height) });

    //Swap back buffer
    SDL_GL_SwapWindow(handle);
}

void Window::setActiveDrawTarget(const Window* w)
{
    SDL_GL_MakeCurrent(w->handle, w->context);
    assert(SDL_GL_GetCurrentWindow () == w->handle);
    assert(SDL_GL_GetCurrentContext() == w->context);
}

void Window::handleEvent(SDL_Event& ev)
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
