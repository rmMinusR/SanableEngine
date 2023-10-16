#pragma once

#include "dllapi.h"

#include "StandardReflectionHooks.hpp"
#include <vector>

#include "game/Component.hpp"

class RectangleCollider : public Component
{
	SANABLE_REFLECTION_HOOKS

private:
	float w, h;

	PRIMITIVES_API RectangleCollider() = default;
public:
	PRIMITIVES_API RectangleCollider(float w, float h);
	PRIMITIVES_API ~RectangleCollider();

	PRIMITIVES_API bool CheckCollision(RectangleCollider const* other) const;

	PRIMITIVES_API bool CheckCollisionAny() const;
	PRIMITIVES_API int GetCollisions(RectangleCollider** outArr = nullptr) const; //It is recommended to allocate this array using the stack allocator
};
