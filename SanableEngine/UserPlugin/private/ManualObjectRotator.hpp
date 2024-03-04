#pragma once

#include "game/Component.hpp"

class ManualObjectRotator : public Component, public IUpdatable
{
	float rotationSpeed;

public:
	ManualObjectRotator();
	ManualObjectRotator(float rotationSpeed);

	virtual void Update() override;
};
