#include "RectangleRenderer.hpp"

#include "EngineCore.hpp"
#include "GameObject.hpp"

#include <SerializationRegistryEntry.hpp>
#include <SerializedObject.hpp>

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

const SerializationRegistryEntry RectangleRenderer::SERIALIZATION_REGISTRY_ENTRY = AUTO_SerializationRegistryEntry(RectangleRenderer, {
	RectangleRenderer* c = MemoryManager::create<RectangleRenderer>(nullptr);
	c->binaryDeserializeMembers(in);
	c->bindGameObject();
});

SerializationRegistryEntry const* RectangleRenderer::getRegistryEntry() const
{
	return &SERIALIZATION_REGISTRY_ENTRY;
}

void RectangleRenderer::binarySerializeMembers(std::ostream& out) const
{
	
}

void RectangleRenderer::binaryDeserializeMembers(std::istream& in)
{
	
}