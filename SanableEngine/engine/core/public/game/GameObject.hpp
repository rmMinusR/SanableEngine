#pragma once

#include "../dllapi.h"

#include <vector>
#include <cassert>
#include <EngineCoreReflectionHooks.hpp>

#include "MemoryManager.hpp"
#include "application/Application.hpp"
#include "Transform.hpp"

class ModuleTypeRegistry;
class Component;
class Application;
struct HotswapTypeData;

class GameObject
{
    SANABLE_REFLECTION_HOOKS

protected:
    Transform transform;

    std::vector<Component*> components;
    friend class Application;
    friend class Component;

    Application* const engine;

    void BindComponent(Component* c);
    void InvokeStart();

public:
    GameObject(Application* engine);
    ~GameObject();

    inline Application* getContext() { return engine; }

    inline Transform* getTransform() { return &transform; }

    template<typename T, typename... TCtorArgs>
    inline T* CreateComponent(const TCtorArgs&... ctorArgs)
    {
        T* component;
        assert((component = GetComponent<T>()) == nullptr);
        component = engine->getMemoryManager()->create<T>(ctorArgs...);
        engine->getGame()->componentAddBuffer.push_back(std::pair<Component*, GameObject*>(component, this));
        return component;
    }

    template<typename T>
    inline T* GetComponent()
    {
        T* out = nullptr;
        for (Component* c : components) if (( out = dynamic_cast<T*>(c) )) return out;
        return nullptr;
    }
};
