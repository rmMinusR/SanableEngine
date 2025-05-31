#pragma once

#include "GameObject.hpp"

class ModuleTypeRegistry;
class GameObject;
class Application;

class Component
{
protected:
	GameObject* gameObject;

	ENGINECORE_API virtual void BindToGameObject(GameObject* obj);
	friend class GameObject;

	ENGINECORE_API inline Game* getEngine() const;

public:
	ENGINECORE_API Component();
	ENGINECORE_API virtual ~Component();

	ENGINECORE_API GameObject* getGameObject() const;
	ENGINECORE_API virtual void onStart();
};


//Interfaces

class Game;
class Level;

class IUpdatable
{
protected:
	virtual void Update() = 0;
	friend class Game;
	friend class Level;
public:
	ENGINECORE_API virtual ~IUpdatable();
};

class Material;
class ShaderProgram;
class GameWindowDispatcher;
class Renderer;
class I3DRenderable
{
public:
	ENGINECORE_API const ShaderProgram* getShader() const;
	virtual const Material* getMaterial() const = 0;
protected:
	virtual void loadModelTransform(Renderer* renderer) const = 0;
	virtual ShaderUniform::ObjectData getRenderedInstanceUniforms() const = 0;
	virtual void renderImmediate(Renderer*) const = 0;
	friend class GameWindowDispatcher;
public:
	ENGINECORE_API virtual ~I3DRenderable();
};
