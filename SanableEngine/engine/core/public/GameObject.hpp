#pragma once

#include "dllapi.h"

#include <vector>
#include <cassert>

#include <ISerializable.hpp>

#include "System.hpp"
#include "Transform.hpp"
#include "MemoryManager.hpp"
#include "EngineCore.hpp"

class Component;
class EngineCore;
struct HotswapTypeData;


class GameObject : public ISerializable
{
protected:
    Transform transform;

    std::vector<Component*> components;
    object_id_t id;

    friend class Component;
    void AddOwnedComponent(Component* c);

    friend class EngineCore;
    friend class Component;

    EngineCore* const engine;

    void BindComponent(Component* c);
    void InvokeStart();

public:
    static object_id_t genID(EngineCore*);
    inline object_id_t getID() const { return id; }
    
    GameObject(EngineCore* engine);
    GameObject(EngineCore* engine, object_id_t id);
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
        component = engine->getMemoryManager()->create<T>(engine, this);
        component->init(initArgs...);
		engine->componentAddBuffer.emplace_back(std::make_pair(component, this));
        return component;
    }

    template<typename T>
    inline T* GetComponent()
    {
        T* out = nullptr;
        for (Component* c : components) if (( out = dynamic_cast<T*>(c) )) return out;
        return nullptr;
    }

protected:
    //Serialization stuff
    static const SerializationRegistryEntry SERIALIZATION_REGISTRY_ENTRY;
    SerializationRegistryEntry const* getRegistryEntry() const override;
    void binarySerializeMembers(std::ostream& out) const override;
    void binaryDeserializeMembers(std::istream& in) override;
};
