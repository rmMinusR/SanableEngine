#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "System.hpp"
#include "EngineCore.hpp"
#include "GameObject.hpp"

#include "RectangleRenderer.hpp"
#include "RectangleCollider.hpp"
#include "ColliderColorChanger.hpp"
#include "PlayerController.hpp"

void userInit(EngineCore* engine);

int main(int argc, char* argv[])
{
    const int WIDTH = 640;
    const int HEIGHT = 480;

    //Init
    SDL_Init(SDL_INIT_VIDEO);
    engine.init("SDL2 Test", WIDTH, HEIGHT, userInit);

    //Loop
    engine.doMainLoop();

    //Shutdown
    engine.shutdown();
    SDL_Quit();

    return 0;
}

void userInit(EngineCore* engine)
{
    GameObject* player = engine->addGameObject();
    player->getTransform()->setPosition(Vector3<float>(50, 50, 0));
    player->CreateComponent<PlayerController>();
    player->CreateComponent<RectangleCollider>(10, 10);
    player->CreateComponent<RectangleRenderer>(10, 10, SDL_Color{ 255, 0, 0, 255 });
    player->CreateComponent<ColliderColorChanger>(SDL_Color{ 255, 0, 0, 255 }, SDL_Color{ 0, 0, 255, 255 });

    GameObject* obstacle = engine->addGameObject();
    obstacle->getTransform()->setPosition(Vector3<float>(200, 200, 0));
    obstacle->CreateComponent<RectangleCollider>(50, 50);
    obstacle->CreateComponent<RectangleRenderer>(50, 50, SDL_Color{ 127, 63, 0, 255 });

    GameObject* staticObj = engine->addGameObject();
    staticObj->getTransform()->setPosition(Vector3<float>(100, 150, 0));
    staticObj->CreateComponent<RectangleRenderer>(510, 120, SDL_Color{ 0, 127, 0, 255 });
}