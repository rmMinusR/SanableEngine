#pragma once

#include "dllapi.h"

#include <SDL_pixels.h>

#include "PluginCore.hpp"
#include "Component.hpp"

class GameObject;

class RectangleRenderer :
	public Component,
	public IRenderable
{
private:
	float w, h;
	SDL_Color color;

public:
	PRIMITIVES_API RectangleRenderer(EngineCore* engine, GameObject* owner);
	PRIMITIVES_API void init(float w, float h, SDL_Color color);
	PRIMITIVES_API ~RectangleRenderer();

	PRIMITIVES_API void Render(Renderer*) override;

	PRIMITIVES_API inline void SetColor(SDL_Color newColor) { color = newColor; }

protected:
	//Serialization stuff
	static const SerializationRegistryEntry SERIALIZATION_REGISTRY_ENTRY;
	SerializationRegistryEntry const* getRegistryEntry() const override;
	void binarySerializeMembers(std::ostream& out) const override;
	void binaryDeserializeMembers(std::istream& in) override;
};
