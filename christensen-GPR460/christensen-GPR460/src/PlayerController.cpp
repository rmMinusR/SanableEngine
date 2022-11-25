#include "PlayerController.hpp"

#include <SDL_keyboard.h>

#include <SerializationRegistryEntry.hpp>
#include <SerializedObject.hpp>

#include "GameObject.hpp"

PlayerController::PlayerController(GameObject* owner) :
	Component(owner)
{
}

void PlayerController::init()
{
	//Nothing to do
}

void PlayerController::Update()
{
	Vector3<float> position = getGameObject()->getTransform()->getPosition();
	
	int sz;
	Uint8 const * keys = SDL_GetKeyboardState(&sz); //Managed by SDL, do not free
	
	//Move position
	if (keys[SDL_SCANCODE_LEFT ]) position += Vector3<float>(-SPEED, 0, 0);
	if (keys[SDL_SCANCODE_RIGHT]) position += Vector3<float>( SPEED, 0, 0);
	if (keys[SDL_SCANCODE_UP   ]) position += Vector3<float>(0, -SPEED, 0);
	if (keys[SDL_SCANCODE_DOWN ]) position += Vector3<float>(0,  SPEED, 0);

	getGameObject()->getTransform()->setPosition(position);
}

const SerializationRegistryEntry PlayerController::SERIALIZATION_REGISTRY_ENTRY = AUTO_Component_SerializationRegistryEntry(PlayerController);

SerializationRegistryEntry const* PlayerController::getRegistryEntry() const
{
	return &SERIALIZATION_REGISTRY_ENTRY;
}

void PlayerController::binarySerializeMembers(std::ostream& out) const
{
	binWriteRaw(0, out); //Dummy val just to make serialization engine realize we're a real object
}

void PlayerController::binaryDeserializeMembers(std::istream& in)
{
}
