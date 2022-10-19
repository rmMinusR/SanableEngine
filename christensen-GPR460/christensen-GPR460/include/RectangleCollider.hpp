#pragma once

#include "Component.hpp"

class RectangleCollider : public Component
{
private:
	float w, h;

public:
	RectangleCollider(GameObject& owner, float w, float h);
	~RectangleCollider();

	bool CheckCollision(RectangleCollider const* other) const;
};
