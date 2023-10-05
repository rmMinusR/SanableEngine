#include <iostream>

#include "PluginCore.hpp"
#include "Plugin.hpp"

#include <SDL.h>
#include "GameObject.hpp"
#include "RectangleRenderer.hpp"
#include "RectangleCollider.hpp"
#include "ColliderColorChanger.hpp"
#include "PlayerController.hpp"
#include "Camera.hpp"

EngineCore* engine;

PLUGIN_C_API(bool) plugin_preInit(Plugin* const context, PluginReportedData* report, EngineCore* engine)
{
    std::cout << "UserPlugin: plugin_preInit() called" << std::endl;
    ::engine = engine;

    report->name = "UserPlugin";


    return true;
}

GameObject* camera;
GameObject* player;
GameObject* obstacle;
GameObject* staticObj;

PLUGIN_C_API(bool) plugin_init(bool firstRun)
{
    std::cout << "UserPlugin: plugin_init() called" << std::endl;

    if (firstRun) {
        camera = engine->addGameObject();
        Camera* cc = camera->CreateComponent<Camera>();
        cc->zFar = 100;
        cc->setGUIProj();
        //cc->setOrtho(400);
        //cc->setPersp(135);
        //camera->getTransform()->setRotation(glm::angleAxis(glm::radians(30.0f), glm::vec3(0, 0, 1)));
        //camera->CreateComponent<PlayerController>();

        GameObject* o = engine->addGameObject();
        o->getTransform()->setPosition(Vector3<float>(-10, -10, 15));
        o->CreateComponent<RectangleRenderer>(100, 100, SDL_Color{ 0, 127, 255, 255 });

        player = engine->addGameObject();
        player->getTransform()->setPosition(Vector3<float>(50, 50, 10));
        player->CreateComponent<PlayerController>();
        player->CreateComponent<RectangleCollider>(100, 100);
        player->CreateComponent<RectangleRenderer>(100, 100, SDL_Color{ 255, 0, 0, 255 });
        player->CreateComponent<ColliderColorChanger>(SDL_Color{ 255, 0, 0, 255 }, SDL_Color{ 0, 0, 255, 255 });

        obstacle = engine->addGameObject();
        obstacle->getTransform()->setPosition(Vector3<float>(200, 200, 20));
        obstacle->CreateComponent<RectangleCollider>(50, 50);
        obstacle->CreateComponent<RectangleRenderer>(50, 50, SDL_Color{ 127, 63, 0, 255 });

        staticObj = engine->addGameObject();
        staticObj->getTransform()->setPosition(Vector3<float>(100, 150, 15));
        staticObj->CreateComponent<RectangleRenderer>(510, 120, SDL_Color{ 0, 127, 0, 255 });
    }

    return true;
}

PLUGIN_C_API(void) plugin_cleanup(bool shutdown)
{
    std::cout << "UserPlugin: plugin_cleanup() called" << std::endl;

    if (shutdown)
    {
        engine->destroy(camera);
        engine->destroy(player);
        engine->destroy(obstacle);
        engine->destroy(staticObj);

        engine->getMemoryManager()->destroyPool<PlayerController>();
        engine->getMemoryManager()->destroyPool<ColliderColorChanger>();
    }
}
