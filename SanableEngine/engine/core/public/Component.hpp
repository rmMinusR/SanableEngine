#pragma once

#include "dllapi.h"

#include "GameObject.hpp"

class ModuleTypeRegistry;
class GameObject;
class EngineCore;

class Component
{
	friend void engine_reportTypes(ModuleTypeRegistry* registry);

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

class Material;
class GameWindowRenderPipeline;
class I3DRenderable
{
	virtual Material* getMaterial() const = 0;
protected:
	virtual void renderImmediate(Renderer*) const = 0;
	friend class GameWindowRenderPipeline;
};
