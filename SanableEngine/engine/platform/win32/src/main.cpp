#include <iostream>

#include <SDL.h>

#include "MemoryRoot.hpp"
#include "application/Application.hpp"
#include "game/Game.hpp"
#include "game/GameWindowRenderPipeline.hpp"
#include "game/GameWindowInputProcessor.hpp"
#include "System_Win32.hpp"
#include "GLSettings.hpp"

int main(int argc, char* argv[])
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    GLSettings glSettings;
    gpr460::System_Win32 system(glSettings);
    Application engine;
    Game game;
    MemoryRoot::get()->registerExternal(&system, ExternalObjectOptions::DefaultExternal);
    MemoryRoot::get()->registerExternal(&engine, ExternalObjectOptions::DefaultExternal);
    MemoryRoot::get()->registerExternal(&game  , ExternalObjectOptions::DefaultExternal);

    //Init
    {
        WindowSettings mainWindowSettings("Sanable Engine", WIDTH, HEIGHT);
        mainWindowSettings.renderPipeline = new GameWindowRenderPipeline(&game);
        mainWindowSettings.inputProcessor = new GameWindowInputProcessor(&game);
        engine.init(&game, mainWindowSettings, system);
    }

    //Loop
    engine.doMainLoop();

    //Shutdown
    engine.shutdown();
    SDL_Quit();
    MemoryRoot::cleanup();
    
    //Pause so we can read console
    system.DebugPause();
    
    return 0;
}
