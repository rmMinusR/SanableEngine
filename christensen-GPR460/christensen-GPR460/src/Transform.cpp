#include "Transform.hpp"

#include <SerializationRegistryEntry.hpp>
#include <SerializedObject.hpp>

#include "EngineCore.hpp"
#include "GameObject.hpp"

Transform::Transform() : Transform(0, 0, 0)
{
}

Transform::Transform(Vector3<num_t> position) :
	position(position)
{
}

Transform::Transform(num_t x, num_t y, num_t z) :
	position(x, y, z)
{
}

Transform::~Transform()
{
}

const SerializationRegistryEntry Transform::SERIALIZATION_REGISTRY_ENTRY = AUTO_SerializationRegistryEntry(Transform, {
	Transform t;
	t.binaryDeserializeMembers(in);
	engine.getGameObject(t.ownerID)->setTransform(t);
});

SerializationRegistryEntry const* Transform::getRegistryEntry() const
{
	return &SERIALIZATION_REGISTRY_ENTRY;
}

void Transform::binarySerializeMembers(std::ostream& out) const
{
	binWriteRaw(position.getX(), out);
	binWriteRaw(position.getY(), out);
	binWriteRaw(position.getZ(), out);
}

void Transform::binaryDeserializeMembers(std::istream& in)
{
	num_t x;
	num_t y;
	num_t z;
	binReadRaw(x, in);
	binReadRaw(y, in);
	binReadRaw(z, in);
	position.set(x, y, z);
}