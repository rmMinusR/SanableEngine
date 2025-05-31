#include <iostream>
#include <emscripten.h>

#include <SDL.h>

#include "MemoryRoot.hpp"
#include "application/Application.hpp"
#include "game/Game.hpp"
#include "game/GameWindowDispatcher.hpp"
#include "System_Emscripten.hpp"

int main(int argc, char* argv[])
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    gpr460::System_Emscripten system;
    Application engine;
    Game game;

    //Init
    {
        GLSettings glSettings;
        WindowSettings mainWindowSettings("Sanable Engine", WIDTH, HEIGHT);
        mainWindowSettings.userLogic = new GameWindowDispatcher(&game);
        engine.init(&game, glSettings, mainWindowSettings, system);
    }

    //Loop
    engine.doMainLoop();

    //NOTE: Due to Emscripten shenanigans, code beyond this point will never run
    //It is here for readability when compared to the Win32 version

    //Shutdown
    engine.shutdown();
    SDL_Quit();
    MemoryRoot::cleanup();

    return 0;
}
