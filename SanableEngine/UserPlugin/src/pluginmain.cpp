#include <iostream>

#include "application/PluginCore.hpp"
#include "application/Plugin.hpp"

#include <SDL.h>
#include "game/Game.hpp"
#include "game/GameObject.hpp"
#include "RectangleRenderer.hpp"
#include "RectangleCollider.hpp"
#include "ColliderColorChanger.hpp"
#include "PlayerController.hpp"
#include "ObjectSpinner.hpp"
#include "ManualObjectRotator.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"
#include "MeshRenderer.hpp"
#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "game/GameWindowRenderPipeline.hpp"

Application* application;

PLUGIN_C_API(bool) plugin_report(Plugin const* context, PluginReportedData* report, Application const* application)
{
    std::cout << "UserPlugin: plugin_report() called" << std::endl;

    report->name = "UserPlugin";

    ::application = (Application*)application; //FIXME bad practice

    return true;
}

GameObject* camera;
GameObject* player;
GameObject* obstacle;
GameObject* staticObj;

Mesh* mesh;
ShaderProgram* shader;
Material* material;

PLUGIN_C_API(bool) plugin_init(bool firstRun)
{
    std::cout << "UserPlugin: plugin_init() called" << std::endl;

    if (firstRun) {
        camera = application->getGame()->addGameObject();
        Camera* cc = camera->CreateComponent<Camera>();
        cc->zFar = 100;
        //cc->setGUIProj();
        //cc->setOrtho(400);
        //cc->setOrtho(1);
        cc->setPersp(90);
        //camera->getTransform()->setRotation(glm::angleAxis(glm::radians(30.0f), glm::vec3(0, 0, 1)));
        camera->CreateComponent<PlayerController>(0.01f);
        camera->CreateComponent<ManualObjectRotator>();

        mesh = new Mesh();
        //mesh->load("resources/bunny.fbx");
        mesh->load("resources/dragon.fbx");

        shader = new ShaderProgram("resources/shaders/fresnel");
        shader->load();
        
        material = new Material(shader);

        for (Vector3f pos(-0.5f, -0.5f, -0.4f); pos.y < 0.5f; pos.y += 0.2f) for (pos.x = -0.5f; pos.x < 0.5f; pos.x += 0.2f)
        {
            GameObject* o = application->getGame()->addGameObject();
            o->getTransform()->setPosition(pos);
            o->CreateComponent<MeshRenderer>(mesh, material);

            Vector3f axis;
            axis.x = (float(rand())/RAND_MAX)*2 - 1;
            axis.y = (float(rand())/RAND_MAX)*2 - 1;
            axis.z = (float(rand())/RAND_MAX)*2 - 1;
            float angle = ((float(rand())/RAND_MAX)*2 - 1) * 0.02f;
            o->CreateComponent<ObjectSpinner>(glm::angleAxis(angle, (glm::vec3)axis));
        }
        //{
        //    GameObject* o = engine->addGameObject();
        //    o->getTransform()->setPosition(Vector3f(0, 0, -0.4f));
        //    o->getTransform()->setLocalScale(Vector3f(10, 10, 10));
        //    o->CreateComponent<MeshRenderer>(mesh, material);
        //    o->CreateComponent<ManualObjectRotator>();
        //}

        player = application->getGame()->addGameObject();
        player->getTransform()->setPosition(Vector3<float>(50, 50, -10));
        //player->CreateComponent<PlayerController>(1);
        player->CreateComponent<RectangleCollider>(10, 10);
        player->CreateComponent<RectangleRenderer>(10, 10, SDL_Color{ 255, 0, 0, 255 });
        player->CreateComponent<ColliderColorChanger>(SDL_Color{ 255, 0, 0, 255 }, SDL_Color{ 0, 0, 255, 255 });

        staticObj = application->getGame()->addGameObject();
        staticObj->getTransform()->setPosition(Vector3<float>(350, 210, -20));
        staticObj->CreateComponent<RectangleRenderer>(510, 120, SDL_Color{ 0, 127, 0, 255 });

        obstacle = application->getGame()->addGameObject();
        obstacle->getTransform()->setPosition(Vector3<float>(225, 225, -15));
        obstacle->CreateComponent<RectangleCollider>(50, 50);
        obstacle->CreateComponent<RectangleRenderer>(50, 50, SDL_Color{ 127, 63, 0, 255 });

        application->buildWindow("Second window", 640, 480, nullptr).build();
    }

    return true;
}

PLUGIN_C_API(void) plugin_cleanup(bool shutdown)
{
    std::cout << "UserPlugin: plugin_cleanup() called" << std::endl;

    if (shutdown)
    {
        application->getGame()->destroy(camera);
        application->getGame()->destroy(player);
        application->getGame()->destroy(obstacle);
        application->getGame()->destroy(staticObj);

        delete mesh;

        application->getMemoryManager()->destroyPool<PlayerController>();
        application->getMemoryManager()->destroyPool<ColliderColorChanger>();
    }
}
