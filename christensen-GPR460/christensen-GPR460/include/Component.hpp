#pragma once

#include <ISerializable.hpp>

class GameObject;

class Component : public ISerializable
{
protected:
	GameObject* const gameObject;

public:
	Component(GameObject* owner);
	virtual ~Component();

	inline GameObject* getGameObject() const { return gameObject; }
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
