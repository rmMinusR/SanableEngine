#pragma once

#include "EngineCoreReflectionHooks.hpp"

#include <vector>
#include "../dllapi.h"
#include "CallBatcher.inl"

class Application;
class GameObject;
class Component;
class IUpdatable;
class I3DRenderable;
class InputSystem;

class Game
{
    SANABLE_REFLECTION_HOOKS

    InputSystem* inputSystem;

    std::vector<GameObject*> objects;
    void applyConcurrencyBuffers();
    std::vector<GameObject*> objectAddBuffer;
    std::vector<GameObject*> objectDelBuffer;
    std::vector<std::pair<Component*, GameObject*>> componentAddBuffer;
    std::vector<Component*> componentDelBuffer;
    friend class GameObject;

    CallBatcher<IUpdatable   > updateList;
    CallBatcher<I3DRenderable> _3dRenderList;

    void refreshCallBatchers();

    void destroyImmediate(GameObject* go);
    void destroyImmediate(Component* c);

    friend class Application;
    void tick();
    void draw();

    Game();
    ~Game();
public:
	ENGINECORE_API InputSystem* getInput();
	ENGINECORE_API const CallBatcher<I3DRenderable>* get3DRenderables();

    ENGINECORE_API GameObject* addGameObject();
    ENGINECORE_API void destroy(GameObject* go);
};
