#include "RectangleRenderer.hpp"

#include "EngineCore.hpp"
#include "GameObject.hpp"

#include <SerializationRegistryEntry.hpp>
#include <SerializedObject.hpp>

RectangleRenderer::RectangleRenderer(GameObject* owner, float w, float h, SDL_Color color) :
	Component(owner),
	w(w),
	h(h),
	color(color)
{
}

RectangleRenderer::~RectangleRenderer()
{
}

void RectangleRenderer::Render()
{
	Vector3<float> pos = gameObject->getTransform()->getPosition();

	SDL_Rect r = {
		(int) pos.getX(),
		(int) pos.getY(),
		(int) w,
		(int) h
	};

	SDL_SetRenderDrawColor(engine.renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(engine.renderer, &r);
}

const SerializationRegistryEntry RectangleRenderer::SERIALIZATION_REGISTRY_ENTRY = AUTO_SerializationRegistryEntry(RectangleRenderer, nullptr, /* nothing to set */, val->getGameObject()->RemoveComponent(val));

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