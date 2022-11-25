#pragma once

#include <SDL_pixels.h>

#include "Component.hpp"

class RectangleCollider;
class RectangleRenderer;

class ColliderColorChanger :
	public Component,
	public IUpdatable
{
private:
	SDL_Color normalColor;
	SDL_Color overlapColor;

	RectangleCollider* collider;
	RectangleRenderer* renderer;

public:
	ColliderColorChanger(GameObject* owner, SDL_Color normalColor, SDL_Color overlapColor);
	~ColliderColorChanger();

	void Update() override;

protected:
	//Serialization stuff
	static const SerializationRegistryEntry SERIALIZATION_REGISTRY_ENTRY;
	SerializationRegistryEntry const* getRegistryEntry() const override;
	void binarySerializeMembers(std::ostream& out) const override;
	void binaryDeserializeMembers(std::istream& in) override;
};
