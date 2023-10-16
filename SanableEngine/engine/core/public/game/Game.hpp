#pragma once

#include "EngineCoreReflectionHooks.hpp"

#include <vector>
#include "../dllapi.h"
#include "CallBatcher.inl"

class Application;
class PluginManager;
class GameObject;
class Component;
class IUpdatable;
class I3DRenderable;
class InputSystem;

class Game
{
    SANABLE_REFLECTION_HOOKS

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

    CallBatcher<IUpdatable   > updateList;
    CallBatcher<I3DRenderable> _3dRenderList;

    void refreshCallBatchers();

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
	ENGINECORE_API const CallBatcher<I3DRenderable>* get3DRenderables();

    ENGINECORE_API GameObject* addGameObject();
    ENGINECORE_API void destroy(GameObject* go);

    ENGINECORE_API inline Application* getApplication() const { return application; }
};
