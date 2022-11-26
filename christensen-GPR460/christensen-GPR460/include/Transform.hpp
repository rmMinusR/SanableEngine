#pragma once

#include <ISerializable.hpp>

#include "Vector3.inl"

#include "Aliases.hpp"

class Transform : public ISerializable
{
public:
	typedef float num_t;

private:
	Vector3<num_t> position;
	object_id_t ownerID;

	friend class GameObject;

public:
	Transform();
	Transform(Vector3<num_t> position);
	Transform(num_t x, num_t y, num_t z);
	~Transform();

	//Getters and setters
	inline Vector3<num_t> getPosition() const { return position; }
	inline void setPosition(Vector3<num_t> v) { position = v; }
	inline void setPosition(num_t x, num_t y, num_t z) { position.set(x, y, z); }

	//Serialization stuff
	static const SerializationRegistryEntry SERIALIZATION_REGISTRY_ENTRY;
	SerializationRegistryEntry const* getRegistryEntry() const override;
	void binarySerializeMembers(std::ostream& out) const override;
	void binaryDeserializeMembers(std::istream& in) override;
};
