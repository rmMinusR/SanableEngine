#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "System.hpp"
#include "EngineCore.hpp"

void frameStep(void* arg);
void runMainLoop(EngineCore* engine);

int main(int argc, char* argv[])
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    SDL_Init(SDL_INIT_VIDEO);
    
    engine.init("SDL2 Test", WIDTH, HEIGHT);
    runMainLoop(&engine);
    engine.shutdown();

    SDL_Quit();

    return 0;
}

void frameStep(void* arg)
{
    EngineCore* engine = (EngineCore*)arg;
    
    engine->tick();
    engine->draw();
}

void runMainLoop(EngineCore* engine)
{
    while (!engine->quit)
    {
        Uint32 now = GetTicks();
        if (now - engine->frameStart >= 16)
        {
            frameStep(engine);
        }
    }
}