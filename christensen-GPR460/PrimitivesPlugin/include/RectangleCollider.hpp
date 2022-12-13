#pragma once

#include "Component.hpp"

#include <vector>

class RectangleCollider : public Component
{
private:
	float w, h;

	static std::vector<RectangleCollider*> REGISTRY; //FIXME remove reliance on std library

public:
	RectangleCollider(float w, float h);
	~RectangleCollider();

	bool CheckCollision(RectangleCollider const* other) const;

	bool CheckCollisionAny() const;
};
