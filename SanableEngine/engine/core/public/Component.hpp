#pragma once

#include "dllapi.h"

#include "GameObject.hpp"

class GameObject;
class EngineCore;

class Component
{
protected:
	GameObject* gameObject;

	ENGINECORE_API virtual void BindToGameObject(GameObject* obj);
	friend class GameObject;

	ENGINECORE_API inline EngineCore* getEngine() const { return gameObject->engine; }

public:
	ENGINECORE_API Component();
	ENGINECORE_API virtual ~Component();

	ENGINECORE_API inline GameObject* getGameObject() const { return gameObject; }
	ENGINECORE_API virtual void onStart();
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
