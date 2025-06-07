#include <iostream>

#include <SDL.h>

#include "MemoryRoot.hpp"
#include "application/Application.hpp"
#include "game/Game.hpp"
#include "game/GameWindowDispatcher.hpp"
#include "EntryPoint.hpp"
#include "System_Win32.hpp"
#include "GLSettings.hpp"

int platformDefaultMain(int argc, char* argv[])
{
    GLSettings glSettings;
    gpr460::System_Win32 system(glSettings);
    Application engine;
    Game game;
    MemoryRoot::get()->registerExternal(&system, ExternalObjectOptions::DefaultExternal);
    MemoryRoot::get()->registerExternal(&engine, ExternalObjectOptions::DefaultExternal);
    MemoryRoot::get()->registerExternal(&game  , ExternalObjectOptions::DefaultExternal);

    //Init
    engine.init(&game, system);

    //Loop
    SanableMain(&engine);
    //engine.doMainLoop();

    //Shutdown
    engine.shutdown();
    SDL_Quit();
    MemoryRoot::cleanup();
    
    //Pause so we can read console
    system.DebugPause();
    
    return 0;
}

#ifndef SANABLE_PLATFORM_NO_IMPLEMENT_MAIN
int main(int argc, char* argv[]) { return platformDefaultMain(argc, argv); }
#endif
