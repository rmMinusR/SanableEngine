#pragma once

#include <vector>
#include "../dllapi.h"
#include "PoolCallBatcher.hpp"

class Application;
class PluginManager;
class GameObject;
class Component;
class IUpdatable;
class I3DRenderable;
class InputSystem;

class Game
{
    Application* application;
    InputSystem* inputSystem;

    std::vector<GameObject*> objects;
    void applyConcurrencyBuffers();
    std::vector<GameObject*> objectAddBuffer;
    std::vector<GameObject*> objectDelBuffer;
    std::vector<std::pair<Component*, GameObject*>> componentAddBuffer;
    std::vector<Component*> componentDelBuffer;
    friend class GameObject;
    friend class PluginManager;

    PoolCallBatcher updateList;
    PoolCallBatcher _3dRenderList;

    void refreshCallBatchers(bool force = false);

    void destroyImmediate(GameObject* go);
    void destroyImmediate(Component* c);

    friend class Application;
    void init(Application* application);
    void cleanup();
    void tick();

    bool isAlive;
public:
    ENGINECORE_API Game();
    ENGINECORE_API ~Game();

    int frame = 0;

	ENGINECORE_API InputSystem* getInput();
	ENGINECORE_API const PoolCallBatcher* get3DRenderables() const;

    ENGINECORE_API GameObject* addGameObject();
    ENGINECORE_API void destroy(GameObject* go);

    ENGINECORE_API inline Application* getApplication() const { return application; }
};
