#pragma once

#include <SDL.h>

#include "System.hpp"

struct EngineState
{
    SDL_Renderer* renderer;
    gpr460::System* system;
    Uint32 frameStart;
    bool quit;
    int frame;
};

void frameStep(void* arg);
Uint32 GetTicks();
