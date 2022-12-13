#pragma once

#include <SDL_pixels.h> //SDL_Color

#include <vector>
#include <cassert>

#include "System.hpp"
#include "Transform.hpp"
#include "MemoryManager.hpp"

class Component;
class EngineCore;

class GameObject
{
protected:
    Transform transform;

    std::vector<Component*> components;
    friend class EngineCore;

    void BindComponent(Component* c);
    void InvokeStart();

public:
    GameObject();
    ~GameObject();

    inline Transform* getTransform() { return &transform; }

    template<typename T, typename... TCtorArgs>
    inline T* CreateComponent(const TCtorArgs&... ctorArgs)
    {
        T* component;
        assert((component = GetComponent<T>()) == nullptr);
        component = MemoryManager::create<T>(ctorArgs...);
        //component = DBG_NEW T(*this, ctorArgs...);
        components.push_back(component);
        return component;
    }

    template<typename T>
    inline T* GetComponent()
    {
        T* out = nullptr;
        for (Component* c : components) if (out = dynamic_cast<T*>(c)) return out;
        return nullptr;
    }
};
