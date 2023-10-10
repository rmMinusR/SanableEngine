#pragma once

#include "Component.hpp"

#include "StandardReflectionHooks.hpp"

#include <glm/glm.hpp>

class ObjectSpinner : public Component, public IUpdatable
{
	SANABLE_REFLECTION_HOOKS

	glm::quat spin;

public:
	ObjectSpinner();
	ObjectSpinner(glm::quat spin);

	virtual void Update() override;
};
