#pragma once

#include "dllapi.h"

#include "Aliases.hpp"

#include <ISerializable.hpp>

#include "GameObject.hpp"

class GameObject;
class EngineCore;

class Component : public ISerializable
{
private:
	EngineCore* engine;
	GameObject* gameObject;
	object_id_t gameObjectID;

protected:
	friend class GameObject;
	ENGINECORE_API virtual void BindToGameObject(GameObject* obj);

	ENGINECORE_API inline EngineCore* getEngine() const { return gameObject->engine; }

public:
	ENGINECORE_API Component(EngineCore* engine);
	ENGINECORE_API Component(EngineCore* engine, GameObject* owner);
	ENGINECORE_API virtual ~Component();

	ENGINECORE_API virtual void onStart();

	ENGINECORE_API inline GameObject* getGameObject() const { return gameObject; }
protected:
	ENGINECORE_API virtual void binarySerializeMembers(std::ostream& out) const override;
	ENGINECORE_API virtual void binaryDeserializeMembers(std::istream& in) override;
};


//Interfaces

class EngineCore;

class IUpdatable
{
protected:
	virtual void Update() = 0;
	friend class EngineCore;
};

class Renderer;
class IRenderable
{
protected:
	virtual void Render(Renderer*) = 0;
	friend class EngineCore;
};


//Serializable
#define AUTO_Component_SerializationRegistryEntry(T) AUTO_SerializationRegistryEntry(T, {  \
    T* c = engine->getMemoryManager()->create<T>(engine, nullptr);						   \
	c->binaryDeserializeMembers(in);													   \
	/*c->bindReferences();*/															   \
})