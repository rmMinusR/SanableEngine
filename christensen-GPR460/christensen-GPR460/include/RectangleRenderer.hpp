#pragma once

#include "Component.hpp"
#include <SDL_pixels.h>

class RectangleRenderer :
	public Component,
	public IRenderable
{
private:
	float w, h;
	SDL_Color color;

public:
	RectangleRenderer(GameObject* owner, float w, float h, SDL_Color color);
	~RectangleRenderer();

	void Render() override;

	inline void SetColor(SDL_Color newColor) { color = newColor; }

protected:
	//Serialization stuff
	static const SerializationRegistryEntry SERIALIZATION_REGISTRY_ENTRY;
	SerializationRegistryEntry const* getRegistryEntry() const override;
	void binarySerializeMembers(std::ostream& out) const override;
	void binaryDeserializeMembers(std::istream& in) override;
};
