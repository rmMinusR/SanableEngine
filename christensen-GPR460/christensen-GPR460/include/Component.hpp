#pragma once

class GameObject;

class Component
{
protected:
	GameObject* gameObject;

	virtual void BindToGameObject(GameObject* obj);
	friend class GameObject;

public:
	Component();
	virtual ~Component();

	inline GameObject* getGameObject() const { return gameObject; }
	virtual void onStart();
};


//Interfaces

class EngineCore;

class IUpdatable
{
protected:
	virtual void Update() = 0;
	friend class EngineCore;
};

class IRenderable
{
protected:
	virtual void Render() = 0;
	friend class EngineCore;
};
