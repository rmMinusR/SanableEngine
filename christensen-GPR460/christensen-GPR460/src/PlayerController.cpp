#include "PlayerController.hpp"

#include "GameObject.hpp"

#include <SDL_keyboard.h>

PlayerController::PlayerController(GameObject* owner) :
	Component(owner)
{
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
