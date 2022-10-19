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
