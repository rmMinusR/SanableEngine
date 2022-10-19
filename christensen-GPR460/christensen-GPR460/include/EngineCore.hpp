#pragma once

#include <SDL.h>

#include "System.hpp"

struct EngineState
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    gpr460::System* system = nullptr;
    Uint32 frameStart = 0;
    bool quit = false;
    int frame = 0;

    EngineState() = default;
};

extern EngineState engine;

void frameStep(void* arg);
Uint32 GetTicks();
