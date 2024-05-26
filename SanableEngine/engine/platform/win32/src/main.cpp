#include <iostream>

#include <SDL.h>

#include "application/Application.hpp"
#include "game/Game.hpp"
#include "game/GameWindowRenderPipeline.hpp"
#include "game/GameWindowInputProcessor.hpp"
#include "System_Win32.hpp"

int main(int argc, char* argv[])
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    gpr460::System_Win32 system;
    Application engine;
    Game game;

    //Init
    {
        GLSettings glSettings;
        WindowBuilder mainWindow = engine.buildWindow("Sanable Engine", WIDTH, HEIGHT);
        mainWindow.setRenderPipeline(new GameWindowRenderPipeline(&game));
        mainWindow.setInputProcessor(new GameWindowInputProcessor(&game));
        engine.init(&game, glSettings, mainWindow, system, nullptr);
    }

    //Loop
    engine.doMainLoop();

    //Shutdown
    engine.shutdown();
    SDL_Quit();
    
    //Pause so we can read console
    system.DebugPause();
    
    return 0;
}
