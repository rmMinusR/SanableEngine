#pragma once

class GameObject;

class Component
{
protected:
	GameObject *const gameObject;

public:
	Component(GameObject& owner);
	virtual ~Component();
};


//Interfaces

class IUpdatable
{
public:
	virtual void Update() = 0;
};

class IRenderable
{
public:
	virtual void Render() = 0;
};
