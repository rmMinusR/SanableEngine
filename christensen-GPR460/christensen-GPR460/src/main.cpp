#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#include "System.hpp"
#include "EngineCore.hpp"

void runMainLoop(EngineState* engine);

int main(int argc, char* argv[])
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    gpr460::System system;
    system.Init();

    //int* leak = DBG_NEW int[4096];

    SDL_Init(SDL_INIT_VIDEO);
    
    engine = EngineState();
    engine.quit = false;
    engine.window = SDL_CreateWindow("SDL2 Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    engine.renderer = SDL_CreateRenderer(engine.window, -1, SDL_RENDERER_ACCELERATED);
    engine.renderPixelFormat = SDL_AllocFormat(SDL_GetWindowPixelFormat(engine.window));
    engine.frame = 0;
    engine.frameStart = GetTicks();
    engine.system = &system;

    runMainLoop(&engine);

    SDL_DestroyRenderer(engine.renderer);
    SDL_DestroyWindow(engine.window);
    engine = EngineState(); //Wipe memory just in case
    SDL_Quit();

    system.Shutdown();
    return 0;
}

void runMainLoop(EngineState* engine)
{
    while (!engine->quit)
    {
        Uint32 now = GetTicks();
        if (now - engine->frameStart >= 16)
        {
            frameStep(engine);
        }
    }
}