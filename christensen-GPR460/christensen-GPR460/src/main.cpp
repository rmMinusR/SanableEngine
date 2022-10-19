#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "System.hpp"
#include "EngineCore.hpp"
#include "GameObject.hpp"

void frameStep(void* arg);
void runMainLoop(EngineCore* engine);

int main(int argc, char* argv[])
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    //Main init
    SDL_Init(SDL_INIT_VIDEO);
    engine.init("SDL2 Test", WIDTH, HEIGHT);

    //User init
    GameObject* staticObj = engine.addGameObject();
    staticObj->getTransform()->setPosition(Vector3<float>(100, 150, 0));
    staticObj->CreateRenderer(510, 120, SDL_Color{ 0, 127, 0, 255 });

    GameObject* obstacle = engine.addGameObject();
    obstacle->getTransform()->setPosition(Vector3<float>(200, 200, 0));
    obstacle->CreateCollider(50, 50);
    obstacle->CreateRenderer(50, 50, SDL_Color{ 127, 63, 0, 255 });

    GameObject* player = engine.addGameObject();
    player->getTransform()->setPosition(Vector3<float>(50, 50, 0));
    player->CreatePlayerController();
    player->CreateCollider(10, 10);
    player->CreateRenderer(10, 10, SDL_Color{ 255, 0, 0, 255 });
    player->CreateColliderColorChanger(SDL_Color{ 255, 0, 0, 255 }, SDL_Color{ 0, 0, 255, 255 });

    //Loop
    runMainLoop(&engine);

    //Shutdown
    engine.shutdown();
    SDL_Quit();

    return 0;
}

void frameStep(void* arg)
{
    EngineCore* engine = (EngineCore*)arg;
    
    engine->tick();
    engine->draw();
}

void runMainLoop(EngineCore* engine)
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