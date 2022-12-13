#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL.h>
#include "EngineCore.hpp"
#include "System_Switcher.hpp"

void vendorInit(EngineCore* engine);
gpr460::System* systemFactory();

int main(int argc, char* argv[])
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    //Init
    SDL_Init(SDL_INIT_VIDEO);
    EngineCore engine;
    engine.init("SDL2 Test", WIDTH, HEIGHT, systemFactory, vendorInit);

    //Loop
    engine.doMainLoop();

    //Shutdown
    engine.shutdown();
    SDL_Quit();

    //Pause so we can read console
#if _WIN32
    system("pause");
#endif

    return 0;
}

gpr460::System* systemFactory()
{
    return new gpr460::System_Impl();
}

void vendorInit(EngineCore* engine)
{
    //Nothing to do here at the moment
}