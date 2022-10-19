#pragma once

#include "Component.hpp"

class PlayerController : public Component
{
private:
	static constexpr float SPEED = 1;

public:
	PlayerController(GameObject& owner);

	void Update(); //Should this take delta tiem?
};
