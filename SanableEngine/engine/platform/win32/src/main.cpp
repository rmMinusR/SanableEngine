#include <iostream>

#include <SDL.h>

#include "EngineCore.hpp"
#include "System_Win32.hpp"
#include "GlobalTypeRegistry.hpp"

void vendorInit(EngineCore* engine);

int main(int argc, char* argv[])
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    gpr460::System_Win32 system;

    //Init
    EngineCore engine;
    engine.init("SDL2 Test", WIDTH, HEIGHT, system, vendorInit);

    //Loop
    engine.doMainLoop();

    //Shutdown
    engine.shutdown();
    SDL_Quit();
    
    //Pause so we can read console
    system.DebugPause();
    
    return 0;
}

void vendorInit(EngineCore* engine)
{
    //Nothing to do here at the moment
}