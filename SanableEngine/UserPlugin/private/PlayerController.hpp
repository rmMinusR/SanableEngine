#pragma once

#include "StandardReflectionHooks.hpp"

#include "game/Component.hpp"

class PlayerController :
	public Component,
	public IUpdatable
{
private:
	SANABLE_REFLECTION_HOOKS

	float moveSpeed;

public:
	PlayerController();
	PlayerController(float moveSpeed);

	void Update() override; //Should this take delta tiem?
};
