#pragma once

#include "StandardReflectionHooks.hpp"

#include "game/Component.hpp"

class ManualObjectRotator : public Component, public IUpdatable
{
	SANABLE_REFLECTION_HOOKS

	float rotationSpeed;

public:
	ManualObjectRotator();
	ManualObjectRotator(float rotationSpeed);

	virtual void Update() override;
};
