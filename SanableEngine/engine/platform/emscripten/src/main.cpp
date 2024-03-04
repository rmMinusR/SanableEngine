#include <iostream>
#include <emscripten.h>

#include <SDL.h>
#include "application/Application.hpp"
#include "game/GameWindowRenderPipeline.hpp"
#include "game/GameWindowInputProcessor.hpp"
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
        WindowBuilder mainWindow = engine.buildWindow("Sanable Engine", WIDTH, HEIGHT, std::make_unique<GameWindowRenderPipeline>(&game));
        mainWindow.setInputProcessor(std::make_unique<GameWindowInputProcessor>(&game));
        engine.init(&game, glSettings, mainWindow, system, nullptr);
    }

    //Loop
    engine.doMainLoop();

    //NOTE: Due to Emscripten shenanigans, code beyond this point will never run
    //It is here for readability when compared to the Win32 version

    //Shutdown
    engine.shutdown();
    SDL_Quit();

    return 0;
}
