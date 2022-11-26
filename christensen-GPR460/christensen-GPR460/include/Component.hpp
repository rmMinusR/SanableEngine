#pragma once

#include "Aliases.hpp"

#include <ISerializable.hpp>

class GameObject;

class Component : public ISerializable
{
private:
	GameObject* gameObject;
	object_id_t gameObjectID;

public:
	Component(GameObject* owner);
	virtual ~Component();

	inline GameObject* getGameObject() const { return gameObject; }
	virtual void bindGameObject(); //Called in deserialization after members

protected:
	virtual void binarySerializeMembers(std::ostream& out) const override;
	virtual void binaryDeserializeMembers(std::istream& in) override;
};


//Interfaces

class EngineCore;

class IUpdatable
{
protected:
	IUpdatable();
	virtual ~IUpdatable();
	virtual void Update() = 0;

	friend class EngineCore;
};

class IRenderable
{
protected:
	IRenderable();
	virtual ~IRenderable();
	virtual void Render() = 0;

	friend class EngineCore;
};


//Serializable
#define AUTO_Component_SerializationRegistryEntry(T) AUTO_SerializationRegistryEntry(T, {  \
    T* c = MemoryManager::create<T>(nullptr);											   \
	c->binaryDeserializeMembers(in);													   \
	c->bindGameObject();																   \
})