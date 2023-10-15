#include <iostream>

#include <SDL.h>

#include "EngineCore.hpp"
#include "GameWindowRenderPipeline.hpp"
#include "System_Win32.hpp"

int main(int argc, char* argv[])
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    gpr460::System_Win32 system;

    //Init
    EngineCore engine;
    GLSettings glSettings;
    WindowBuilder mainWindow = engine.buildWindow("Sanable Engine", WIDTH, HEIGHT, std::make_unique<GameWindowRenderPipeline>(&engine));
    engine.init(glSettings, mainWindow, system, nullptr);

    //Loop
    engine.doMainLoop();

    //Shutdown
    engine.shutdown();
    SDL_Quit();
    
    //Pause so we can read console
    system.DebugPause();
    
    return 0;
}
