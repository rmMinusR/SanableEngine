#pragma once

#include <vector>
#include <cassert>

#include "Transform.hpp"
#include "MemoryManager.hpp"
#include "EngineCore.hpp"

class Component;
class EngineCore;
struct HotswapTypeData;

class GameObject
{
protected:
    Transform transform;

    struct ComponentRecord
    {
        Component* ptr;
        std::string name;
    };

    std::vector<ComponentRecord> components;
    friend class EngineCore;
    friend class Component;

    EngineCore* const engine;

    void BindComponent(Component* c);
    void InvokeStart();

public:
    GameObject(EngineCore* engine);
    ~GameObject();

    inline Transform* getTransform() { return &transform; }

    template<typename T, typename... TCtorArgs>
    inline T* CreateComponent(const TCtorArgs&... ctorArgs)
    {
        T* component;
        assert((component = GetComponent<T>()) == nullptr);
        component = engine->getMemoryManager()->create<T>(ctorArgs...);
        engine->componentAddBuffer.push_back(std::pair<Component*, GameObject*>(component, this));
        return component;
    }

    template<typename T>
    inline T* GetComponent()
    {
        T* out = nullptr;
        for (ComponentRecord& c : components) if (out = dynamic_cast<T*>(c.ptr)) return out;
        return nullptr;
    }

    void refreshVtables(std::vector<HotswapTypeData*>& refreshers);
};
