#include <iostream>
#include <emscripten.h>

#include <SDL.h>
#include "EngineCore.hpp"
#include "System_Emscripten.hpp"

void vendorInit(EngineCore* engine);

int main(int argc, char* argv[])
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    gpr460::System_Emscripten system;

    //Init
    EngineCore engine;
    engine.init("Sanable Engine", WIDTH, HEIGHT, system, nullptr);

    //Loop
    engine.doMainLoop();

    //NOTE: Due to Emscripten shenanigans, code beyond this point will never run
    //It is here for readability when compared to the Win32 version

    //Shutdown
    engine.shutdown();
    SDL_Quit();

    return 0;
}
