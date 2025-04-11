#pragma once

#include "Color.inl"
#include "game/Component.hpp"

class RectangleCollider;
class RectangleRenderer;

class ColliderColorChanger :
	public Component,
	public IUpdatable
{
private:
	Color4<uint8_t> normalColor;
	Color4<uint8_t> overlapColor;

	RectangleCollider* collider;
	RectangleRenderer* renderer;

	ColliderColorChanger() = default;
protected:
	void BindToGameObject(GameObject* obj) override;

public:
	ColliderColorChanger(Color4<uint8_t> normalColor, Color4<uint8_t> overlapColor);
	~ColliderColorChanger();

	void Update() override;
};
