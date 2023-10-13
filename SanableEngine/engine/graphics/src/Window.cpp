#include "Window.hpp"

#include <SDL.h>
#include <GL/glew.h>
#include "GLContext.hpp"
#include "WindowRenderPipeline.hpp"
#include "EngineCore.hpp"

Window::Window(const std::string& name, int width, int height, EngineCore* engine, std::unique_ptr<WindowRenderPipeline>&& renderPipeline) :
    engine(engine),
    renderPipeline(std::move(renderPipeline))
{
    SDL_InitSubSystem(SDL_INIT_VIDEO); //Internally refcounted, no checks necessary

    //OpenGL setup: Specify v3.1 (latest with fixed-function pipeline)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    //OpenGL setup: Use RGBA8 with 16-bit depth
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //Instead of passing SDL_DOUBLEBUF to SDL_SetVideoMode

    handle = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    context = GLContext::create(handle, this);
    
    _interface = Renderer(this, context);
    
    printf("Window '%s' - OpenGL %s\n", name.c_str(), (char*)glGetString(GL_VERSION));
}

Window::~Window()
{
    GLContext::release(context, this);

    SDL_DestroyWindow(handle);
    handle = nullptr;

    SDL_QuitSubSystem(SDL_INIT_VIDEO); //Internally refcounted, no checks necessary
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
    SDL_GL_MakeCurrent(handle, context);
    glViewport(0, 0, getWidth(), getHeight());

    //Delegate draw
    renderPipeline->render();

    //Swap back buffer
    SDL_GL_SwapWindow(handle);
}
