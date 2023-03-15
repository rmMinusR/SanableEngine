#include "Window.hpp"

#include <SDL_render.h>

Window::Window(char const* name, int width, int height)
{
    handle = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(handle, -1, SDL_RENDERER_ACCELERATED);

    _interface = Renderer(renderer);
}

Window::~Window()
{
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;

    SDL_DestroyWindow(handle);
    handle = nullptr;
}
