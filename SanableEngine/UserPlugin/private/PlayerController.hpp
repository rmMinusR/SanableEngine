#pragma once

#include "game/Component.hpp"

class PlayerController :
	public Component,
	public IUpdatable
{
private:
	float moveSpeed;

public:
	PlayerController();
	PlayerController(float moveSpeed);

	void Update() override; //Should this take delta tiem?
};
