#pragma once

#include <SDL_pixels.h>

#include "Component.hpp"

#include "dllapi.h"

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

protected:
	void BindToGameObject(GameObject* obj) override;

public:
	ColliderColorChanger(EngineCore* engine, GameObject* obj);
	void init(SDL_Color normalColor, SDL_Color overlapColor);
	~ColliderColorChanger();

	void Update() override;

protected:
	//Serialization stuff
	static const SerializationRegistryEntry SERIALIZATION_REGISTRY_ENTRY;
	SerializationRegistryEntry const* getRegistryEntry() const override;
	void binarySerializeMembers(std::ostream& out) const override;
	void binaryDeserializeMembers(std::istream& in) override;
};
