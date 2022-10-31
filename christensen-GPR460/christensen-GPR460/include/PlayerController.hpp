#pragma once

#include "Component.hpp"

class PlayerController :
	public Component,
	public IUpdatable
{
private:
	static constexpr float SPEED = 1;

public:
	PlayerController(GameObject& owner);

	void Update() override; //Should this take delta tiem?
};
