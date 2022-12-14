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

PLUGIN_C_API(bool) plugin_preInit(Plugin* const context, PluginReportedData* report, EngineCore* engine)
{
    std::cout << "UserPlugin: plugin_preInit() called" << std::endl;
    ::engine = engine;

    report->name = "UserPlugin";

    report->hotswappables.push_back(HotswapTypeData::build<ColliderColorChanger>(SDL_Color{}, SDL_Color{}));
    report->hotswappables.push_back(HotswapTypeData::build<PlayerController>());

    return true;
}

GameObject* player;
GameObject* obstacle;
GameObject* staticObj;

PLUGIN_C_API(bool) plugin_init()
{
    std::cout << "UserPlugin: plugin_init() called" << std::endl;

    player = engine->addGameObject();
    player->getTransform()->setPosition(Vector3<float>(50, 50, 0));
    player->CreateComponent<PlayerController>();
    player->CreateComponent<RectangleCollider>(10, 10);
    player->CreateComponent<RectangleRenderer>(10, 10, SDL_Color{ 255, 0, 0, 255 });
    player->CreateComponent<ColliderColorChanger>(SDL_Color{ 255, 0, 0, 255 }, SDL_Color{ 0, 0, 255, 255 });

    obstacle = engine->addGameObject();
    obstacle->getTransform()->setPosition(Vector3<float>(200, 200, 0));
    obstacle->CreateComponent<RectangleCollider>(50, 50);
    obstacle->CreateComponent<RectangleRenderer>(50, 50, SDL_Color{ 127, 63, 0, 255 });

    staticObj = engine->addGameObject();
    staticObj->getTransform()->setPosition(Vector3<float>(100, 150, 0));
    staticObj->CreateComponent<RectangleRenderer>(510, 120, SDL_Color{ 0, 127, 0, 255 });

    return true;
}

PLUGIN_C_API(void) plugin_cleanup()
{
    std::cout << "UserPlugin: plugin_cleanup() called" << std::endl;

    engine->destroy(player);
    engine->destroy(obstacle);
    engine->destroy(staticObj);
}
