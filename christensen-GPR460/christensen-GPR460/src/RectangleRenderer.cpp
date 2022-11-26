#include "RectangleRenderer.hpp"

#include <SerializationRegistryEntry.hpp>
#include <SerializedObject.hpp>

#include "EngineCore.hpp"
#include "GameObject.hpp"

RectangleRenderer::RectangleRenderer(GameObject* owner) :
	Component(owner)
{
}

void RectangleRenderer::init(float w, float h, SDL_Color color)
{
	this->w = w;
	this->h = h;
	this->color = color;
}

RectangleRenderer::~RectangleRenderer()
{
}

void RectangleRenderer::Render()
{
	Vector3<float> pos = getGameObject()->getTransform()->getPosition();

	SDL_Rect r = {
		(int) pos.getX(),
		(int) pos.getY(),
		(int) w,
		(int) h
	};

	SDL_SetRenderDrawColor(engine.renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(engine.renderer, &r);
}

const SerializationRegistryEntry RectangleRenderer::SERIALIZATION_REGISTRY_ENTRY = AUTO_Component_SerializationRegistryEntry(RectangleRenderer);

SerializationRegistryEntry const* RectangleRenderer::getRegistryEntry() const
{
	return &SERIALIZATION_REGISTRY_ENTRY;
}

void RectangleRenderer::binarySerializeMembers(std::ostream& out) const
{
	Component::binarySerializeMembers(out);

	binWriteRaw(w, out);
	binWriteRaw(h, out);
	binWriteRaw(color, out);
}

void RectangleRenderer::binaryDeserializeMembers(std::istream& in)
{
	Component::binaryDeserializeMembers(in);

	binReadRaw(w, in);
	binReadRaw(h, in);
	binReadRaw(color, in);
}
