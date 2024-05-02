#pragma once

#include <vector>
#include "../dllapi.h"
#include "Component.hpp"
#include "PoolCallBatcher.hpp"

class Application;
class PluginManager;
class GameObject;
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

    PoolCallBatcher<IUpdatable> updateList;
    PoolCallBatcher<I3DRenderable> _3dRenderList;

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
	ENGINECORE_API const PoolCallBatcher<I3DRenderable>* get3DRenderables() const;

    ENGINECORE_API GameObject* addGameObject();
    ENGINECORE_API void destroy(GameObject* go);

    ENGINECORE_API inline Application* getApplication() const { return application; }
};
