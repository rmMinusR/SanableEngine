#pragma once

#include <vector>

#include "Component.hpp"
#include "PluginCore.hpp"

class RectangleCollider : public Component
{
private:
	float w, h;

public:
	PLUGIN_API_CTOR RectangleCollider(float w, float h);
	~RectangleCollider();

	PLUGIN_API(bool) CheckCollision(RectangleCollider const* other) const;

	PLUGIN_API(bool) CheckCollisionAny() const;
};
