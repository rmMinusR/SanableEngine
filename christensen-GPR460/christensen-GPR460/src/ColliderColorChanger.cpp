#include "ColliderColorChanger.hpp"

#include <cassert>

#include <SerializationRegistryEntry.hpp>
#include <SerializedObject.hpp>

#include "GameObject.hpp"
#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"

ColliderColorChanger::ColliderColorChanger(GameObject* owner) :
	Component(owner)
{
}

void ColliderColorChanger::init(SDL_Color normalColor, SDL_Color overlapColor)
{
	this->normalColor = normalColor;
	this->overlapColor = overlapColor;
}

void ColliderColorChanger::bindGameObject()
{
	Component::bindGameObject();

	collider = getGameObject()->GetComponent<RectangleCollider>();
	renderer = getGameObject()->GetComponent<RectangleRenderer>();

	assert(collider);
	assert(renderer);
}

void ColliderColorChanger::Update()
{
	renderer->SetColor(collider->CheckCollisionAny() ? overlapColor : normalColor);
}

const SerializationRegistryEntry ColliderColorChanger::SERIALIZATION_REGISTRY_ENTRY = AUTO_Component_SerializationRegistryEntry(ColliderColorChanger);

SerializationRegistryEntry const* ColliderColorChanger::getRegistryEntry() const
{
	return &SERIALIZATION_REGISTRY_ENTRY;
}

void ColliderColorChanger::binarySerializeMembers(std::ostream& out) const
{
	binWriteRaw(normalColor, out);
	binWriteRaw(overlapColor, out);
}

void ColliderColorChanger::binaryDeserializeMembers(std::istream& in)
{
	binReadRaw(normalColor, in);
	binReadRaw(overlapColor, in);
}