#pragma once

#include "dllapi.h"

#include <vector>

#include "Component.hpp"

class RectangleCollider : public Component
{
private:
	float w, h;

public:
	PRIMITIVES_API RectangleCollider(float w, float h);
	PRIMITIVES_API ~RectangleCollider();

	PRIMITIVES_API bool CheckCollision(RectangleCollider const* other) const;

	PRIMITIVES_API bool CheckCollisionAny() const;
	PRIMITIVES_API int GetCollisions(RectangleCollider** outArr = nullptr) const; //It is recommended to allocate this array using the stack allocator
};
