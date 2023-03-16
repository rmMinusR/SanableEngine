#pragma once

#include "Component.hpp"

class PlayerController :
	public Component,
	public IUpdatable
{
private:
	static constexpr float SPEED = 1;

public:
	PlayerController(EngineCore* engine, GameObject* owner);
	inline void init() {}

	void Update() override; //Should this take delta tiem?

protected:
	//Serialization stuff
	static const SerializationRegistryEntry SERIALIZATION_REGISTRY_ENTRY;
	SerializationRegistryEntry const* getRegistryEntry() const override;
	void binarySerializeMembers(std::ostream& out) const override;
	void binaryDeserializeMembers(std::istream& in) override;
};
