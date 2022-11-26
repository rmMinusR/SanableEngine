#pragma once

#include <SDL_pixels.h> //SDL_Color

#include <vector>
#include <cassert>

#include <ISerializable.hpp>

#include "System.hpp"
#include "Transform.hpp"
#include "MemoryManager.hpp"

class Component;


class GameObject : public ISerializable
{
protected:
    Transform transform;
    std::vector<Component*> components;
    object_id_t id;

    friend class Component;
    void AddOwnedComponent(Component* c);

public:
    static object_id_t genID();
    inline object_id_t getID() const { return id; }
    
    GameObject();
    GameObject(object_id_t id);
    ~GameObject();

    inline Transform* getTransform() { return &transform; }
    inline void setTransform(const Transform& cpy) {
        transform.setPosition(cpy.getPosition());
    }

    template<typename T, typename... TCtorArgs>
    inline T* CreateComponent(const TCtorArgs&... initArgs)
    {
        T* component;
        assert((component = GetComponent<T>()) == nullptr);
        component = MemoryManager::create<T>(this);
        component->init(initArgs...);
        component->bindGameObject(); //Chain calls AddOwnedComponent
        return component;
    }

    template<typename T>
    inline T* GetComponent()
    {
        T* out = nullptr;
        for (Component* c : components) if (out = dynamic_cast<T*>(c)) return out;
        return nullptr;
    }

    void RemoveComponent(Component* c);

protected:
    //Serialization stuff
    static const SerializationRegistryEntry SERIALIZATION_REGISTRY_ENTRY;
    SerializationRegistryEntry const* getRegistryEntry() const override;
    void binarySerializeMembers(std::ostream& out) const override;
    void binaryDeserializeMembers(std::istream& in) override;
};
