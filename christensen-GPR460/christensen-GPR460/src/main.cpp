#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "System.hpp"
#include "EngineCore.hpp"
#include "MemoryManager.hpp"

#include "GameObject.hpp"
#include "RectangleRenderer.hpp"
#include "RectangleCollider.hpp"
#include "ColliderColorChanger.hpp"
#include "PlayerController.hpp"

void userInit(EngineCore* engine);
void userInit2(EngineCore* engine);
void blankUserInit(EngineCore* engine) {}

const int WIDTH = 640;
const int HEIGHT = 480;


int main(int argc, char* argv[])
{
    srand(time(nullptr));

    //Init
    SDL_Init(SDL_INIT_VIDEO);
    engine.init("SDL2 Test", WIDTH, HEIGHT, blankUserInit);
    //engine.init("SDL2 Test", WIDTH, HEIGHT, userInit);

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
    player->getTransform()->setPosition(Vector3<float>(WIDTH/2, HEIGHT/2, 0));
    player->CreateComponent<PlayerController>();
    player->CreateComponent<RectangleCollider>(10, 10);
    player->CreateComponent<RectangleRenderer>(10, 10, SDL_Color{ 255, 0, 0, 255 });
    player->CreateComponent<ColliderColorChanger>(SDL_Color{ 255, 0, 0, 255 }, SDL_Color{ 0, 0, 255, 255 });

    for (int i = 0; i < 8; ++i)
    {
        GameObject* staticObj = engine->addGameObject();
        staticObj->getTransform()->setPosition(Vector3<float>(rand()%WIDTH, rand()%HEIGHT, 0));
        int w = 25+rand()%50;
        int h = 25+rand()%50;
        staticObj->CreateComponent<RectangleRenderer>(w, h, SDL_Color{ 0, 127, 0, 255 });
        staticObj->CreateComponent<RectangleCollider>(w, h);
    }
}

void userInit2(EngineCore* engine)
{
    GameObject* player = engine->addGameObject();
    player->getTransform()->setPosition(Vector3<float>(50, 50, 0));
    player->CreateComponent<PlayerController>();
    player->CreateComponent<RectangleRenderer>(10, 10, SDL_Color{ 255, 0, 0, 255 });
    //player->CreateComponent<RectangleCollider>(10, 10);
    //player->CreateComponent<ColliderColorChanger>(SDL_Color{ 255, 0, 0, 255 }, SDL_Color{ 0, 0, 255, 255 });
}