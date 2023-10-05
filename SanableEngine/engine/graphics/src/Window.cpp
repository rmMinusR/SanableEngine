#include "Window.hpp"

#undef WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <SDL.h>
#include <gl/GL.h>

Window::Window(char const* name, int width, int height)
{
    SDL_InitSubSystem(SDL_INIT_VIDEO); //Internally refcounted, no checks necessary

    //OpenGL setup: Specify v3.1 (lastest with fixed-function pipeline)
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

    handle = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(handle);

    _interface = Renderer(this, context);

    printf("OpenGL ");
    printf((char*)glGetString(GL_VERSION));
    printf("\n");
}

Window::~Window()
{
    SDL_DestroyWindow(handle);
    handle = nullptr;

    SDL_QuitSubSystem(SDL_INIT_VIDEO); //Internally refcounted, no checks necessary
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
