#pragma once

#include "dllapi.h"

#include <vector>

#include "Component.hpp"

class RectangleCollider : public Component
{
private:
	float w, h;

public:
	PRIMITIVES_API RectangleCollider(EngineCore* engine, GameObject* owner);
	PRIMITIVES_API void init(float w, float h);
	PRIMITIVES_API ~RectangleCollider();

	PRIMITIVES_API bool CheckCollision(RectangleCollider const* other) const;

	PRIMITIVES_API bool CheckCollisionAny() const;

protected:
	//Serialization stuff
	static const SerializationRegistryEntry SERIALIZATION_REGISTRY_ENTRY;
	SerializationRegistryEntry const* getRegistryEntry() const override;
	void binarySerializeMembers(std::ostream& out) const override;
	void binaryDeserializeMembers(std::istream& in) override;
};
