#pragma once

#include "Component.hpp"

#include "StandardReflectionHooks.hpp"

class ManualObjectRotator : public Component, public IUpdatable
{
	SANABLE_REFLECTION_HOOKS

	float rotationSpeed;

public:
	ManualObjectRotator();
	ManualObjectRotator(float rotationSpeed);

	virtual void Update() override;
};
