#pragma once

#include <glm/glm.hpp>

#include "game/Component.hpp"

class ObjectSpinner : public Component, public IUpdatable
{
	glm::quat spin;

public:
	ObjectSpinner();
	ObjectSpinner(glm::quat spin);

	virtual void Update() override;
};
