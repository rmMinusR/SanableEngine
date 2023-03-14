#include <iostream>
#include <emscripten.h>

#include <SDL.h>
#include "EngineCore.hpp"
#include "System_Emscripten.hpp"

void vendorInit(EngineCore* engine);
gpr460::System* systemFactory();

int main(int argc, char* argv[])
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    gpr460::System_Emscripten system;

    //Init
    SDL_Init(SDL_INIT_VIDEO);
    EngineCore engine;
    engine.init("SDL2 Test", WIDTH, HEIGHT, system, vendorInit);

    //Loop
    engine.doMainLoop();

    //NOTE: Due to Emscripten shenanigans, code beyond this point will never run
    //It is here for readability when compared to the Win32 version

    //Shutdown
    engine.shutdown();
    SDL_Quit();

    return 0;
}

void vendorInit(EngineCore* engine)
{
    //Nothing to do here at the moment
}