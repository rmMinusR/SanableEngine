#include "application/Window.hpp"

#include <SDL.h>
#include <GL/glew.h>
#include "GLContext.hpp"
#include "application/WindowRenderPipeline.hpp"
#include "application/Application.hpp"
#include "GLSettings.hpp"

Window::Window() :
    handle(nullptr),
    context(nullptr),
    engine(nullptr)
{
    //Reflection hooks compat
}

Window::Window(const std::string& name, int width, int height, const GLSettings& glSettings, Application* engine, std::unique_ptr<WindowRenderPipeline>&& renderPipeline, std::unique_ptr<WindowInputProcessor>&& inputProcessor) :
    engine(engine),
    renderPipeline(std::move(renderPipeline)),
    inputProcessor(std::move(inputProcessor))
{
    SDL_InitSubSystem(SDL_INIT_VIDEO); //Internally refcounted, no checks necessary

    glSettings.apply();

    handle = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    context = GLContext::create(handle, this);
    
    _interface = Renderer(this, context);
    
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

void Window::draw() const
{
    //Reset to default state
    setActiveDrawTarget(this);
    int width, height;
    SDL_GetWindowSize(handle, &width, &height);
    glViewport(0, 0, width, height);

    //Delegate draw
    if (renderPipeline) renderPipeline->render({ 0, 0, (float)width, (float)height });

    //Swap back buffer
    SDL_GL_SwapWindow(handle);
}

void Window::setActiveDrawTarget(const Window* w)
{
    SDL_GL_MakeCurrent(w->handle, w->context);
    assert(SDL_GL_GetCurrentWindow () == w->handle);
    assert(SDL_GL_GetCurrentContext() == w->context);
}

void Window::handleEvent(SDL_Event& ev) const
{
    if (inputProcessor) inputProcessor->handleEvent(ev);
}
