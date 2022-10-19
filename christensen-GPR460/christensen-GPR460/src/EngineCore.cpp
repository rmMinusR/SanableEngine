#include "EngineCore.hpp"

#include <iostream>

#include "GameObject.hpp"

EngineCore engine;

Uint32 GetTicks()
{
    return SDL_GetTicks();
}

void EngineCore::processEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) quit = true;

        if (event.type == SDL_KEYDOWN)
        {
            std::cout << "Key pressed!\n";
            if (event.key.keysym.sym == SDLK_k)
            {
                std::cout << "K pressed!\n";

                // TODO: Add calls to ErrorMessage and LogToErrorFile here
            }
            if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
        }
    }
}

EngineCore::EngineCore()
{
}

EngineCore::~EngineCore()
{
}

void EngineCore::tick()
{
    frame++;
    frameStart = GetTicks();

    processEvents();

    for (GameObject* o : objects) o->Update();
}

void EngineCore::init(char const* windowName, int windowWidth, int windowHeight)
{
    system.Init();

    quit = false;
    window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    frame = 0;
    frameStart = GetTicks();
}

void EngineCore::draw()
{
    //Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    //Draw objects
    for (GameObject* o : objects) o->Render();

    SDL_RenderPresent(renderer);
}

void EngineCore::shutdown()
{
    for (GameObject* o : objects) delete o;
    objects.clear();

    SDL_DestroyRenderer(renderer);
    renderer = nullptr;

    SDL_DestroyWindow(window);
    window = nullptr;

    system.Shutdown();
}
