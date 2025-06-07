#include <iostream>
#include <emscripten.h>

#include <SDL.h>

#include "MemoryRoot.hpp"
#include "application/Application.hpp"
#include "game/Game.hpp"
#include "game/GameWindowDispatcher.hpp"
#include "EntryPoint.hpp"
#include "System_Emscripten.hpp"

int platformDefaultMain(int argc, char* argv[])
{
    gpr460::System_Emscripten system;
    Application engine;
    Game game;

    //Init
    engine.init(&game, system);
    
    //Loop
    SanableMain(&engine);
    //engine.doMainLoop();

    //NOTE: Due to Emscripten shenanigans, code beyond this point will never run
    //It is here for readability when compared to the Win32 version

    //Shutdown
    engine.shutdown();
    SDL_Quit();
    MemoryRoot::cleanup();

    return 0;
}

#ifndef SANABLE_PLATFORM_NO_IMPLEMENT_MAIN
int main(int argc, char* argv[]) { return platformDefaultMain(argc, argv); }
#endif
