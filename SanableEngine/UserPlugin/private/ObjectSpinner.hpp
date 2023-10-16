#pragma once

#include "StandardReflectionHooks.hpp"

#include <glm/glm.hpp>

#include "game/Component.hpp"

class ObjectSpinner : public Component, public IUpdatable
{
	SANABLE_REFLECTION_HOOKS

	glm::quat spin;

public:
	ObjectSpinner();
	ObjectSpinner(glm::quat spin);

	virtual void Update() override;
};
