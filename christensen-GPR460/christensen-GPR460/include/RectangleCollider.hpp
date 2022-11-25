#pragma once

#include "Component.hpp"

#include <vector>

class RectangleCollider : public Component
{
private:
	float w, h;

	static std::vector<RectangleCollider*> REGISTRY; //FIXME remove reliance on std library

public:
	RectangleCollider(GameObject* owner);
	void init(float w, float h);
	~RectangleCollider();

	bool CheckCollision(RectangleCollider const* other) const;

	bool CheckCollisionAny() const;

protected:
	//Serialization stuff
	static const SerializationRegistryEntry SERIALIZATION_REGISTRY_ENTRY;
	SerializationRegistryEntry const* getRegistryEntry() const override;
	void binarySerializeMembers(std::ostream& out) const override;
	void binaryDeserializeMembers(std::istream& in) override;
};
