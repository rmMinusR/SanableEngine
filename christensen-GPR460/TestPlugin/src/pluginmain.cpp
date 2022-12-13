#include <iostream>

#include "PluginCore.hpp"
#include "Plugin.hpp"

#include <SDL.h>
#include "GameObject.hpp"
#include "RectangleRenderer.hpp"
#include "RectangleCollider.hpp"
#include "ColliderColorChanger.hpp"
#include "PlayerController.hpp"

EngineCore* engine;

PLUGIN_API(bool) plugin_preInit(Plugin* context, EngineCore* engine)
{
    std::cout << "TestPlugin: plugin_preInit() called" << std::endl;
    ::engine = engine;
    return true;
}

PLUGIN_API(bool) plugin_init()
{
    std::cout << "TestPlugin: plugin_init() called" << std::endl;

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

    return true;
}

PLUGIN_API(void) plugin_cleanup()
{
    std::cout << "TestPlugin: plugin_cleanup() called" << std::endl;
}

PLUGIN_API(int) testExport()
{
    std::cout << "TestPlugin: testExport() called" << std::endl;
    return 42;
}
