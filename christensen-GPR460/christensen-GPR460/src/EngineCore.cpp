#include "EngineCore.hpp"

#include <iostream>

EngineState engine;

void frameStep(void* arg)
{
    EngineState* engine = (EngineState*)arg;
    SDL_Event event;

    Uint32 now = GetTicks();
    
    engine->frame++;
    engine->frameStart = now;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            engine->quit = true;
        }

        if (event.type == SDL_KEYDOWN)
        {
            std::cout << "Key pressed!\n";
            if (event.key.keysym.sym == SDLK_k)
            {
                std::cout << "K pressed!\n";

                // TODO: Add calls to ErrorMessage and LogToErrorFile here
            }
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                engine->quit = true;
            }
        }
    }

    int x = (SDL_sinf(engine->frame / 100.0f) * 100.0f) + 200;

    SDL_Rect r = {
        x,
        100,
        50,
        50
    };

    SDL_SetRenderDrawColor(engine->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    SDL_RenderClear(engine->renderer);
    SDL_SetRenderDrawColor(engine->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(engine->renderer, &r);
    SDL_RenderPresent(engine->renderer);
}

Uint32 GetTicks()
{
    return SDL_GetTicks();
}