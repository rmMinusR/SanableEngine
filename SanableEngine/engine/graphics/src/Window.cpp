#include "Window.hpp"

#include <SDL.h>

Window::Window(char const* name, int width, int height)
{
    SDL_InitSubSystem(SDL_INIT_VIDEO);

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

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
