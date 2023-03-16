#include "ColliderColorChanger.hpp"

#include "SerializationRegistryEntry.hpp"
#include "GameObject.hpp"
#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"

void ColliderColorChanger::BindToGameObject(GameObject* obj)
{
	Component::BindToGameObject(obj);

	collider = obj->GetComponent<RectangleCollider>();
	renderer = obj->GetComponent<RectangleRenderer>();

	assert(collider);
	assert(renderer);
}

ColliderColorChanger::ColliderColorChanger(EngineCore* engine, GameObject* obj) :
	Component(engine, obj),
	collider(nullptr),
	renderer(nullptr)
{
}

ColliderColorChanger::~ColliderColorChanger()
{
}

void ColliderColorChanger::init(SDL_Color normalColor, SDL_Color overlapColor)
{
	this->normalColor = normalColor;
	this->overlapColor = overlapColor;
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
	Component::binarySerializeMembers(out);

	binWriteRaw(normalColor, out);
	binWriteRaw(overlapColor, out);
}

void ColliderColorChanger::binaryDeserializeMembers(std::istream& in)
{
	Component::binaryDeserializeMembers(in);

	collider = getGameObject()->GetComponent<RectangleCollider>();
	renderer = getGameObject()->GetComponent<RectangleRenderer>();

	binReadRaw(normalColor, in);
	binReadRaw(overlapColor, in);
}