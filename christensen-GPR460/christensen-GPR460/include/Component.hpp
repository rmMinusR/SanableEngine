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
	void bindGameObject(); //Called in deserialization after members
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
