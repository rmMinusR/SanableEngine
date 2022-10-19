#include "PlayerController.hpp"

#include "GameObject.hpp"

#include <SDL_keyboard.h>

PlayerController::PlayerController(GameObject& owner) :
	Component(owner)
{
}

void PlayerController::Update()
{
	Vector3<float> position = gameObject->getTransform()->getPosition();
	
	int sz;
	Uint8 const * keys = SDL_GetKeyboardState(&sz); //Managed by SDL, do not free
	
	//Move position
	if (keys[SDLK_LEFT ]) position += Vector3<float>(-SPEED, 0, 0);
	if (keys[SDLK_RIGHT]) position += Vector3<float>( SPEED, 0, 0);
	if (keys[SDLK_UP   ]) position += Vector3<float>(0, -SPEED, 0);
	if (keys[SDLK_DOWN ]) position += Vector3<float>(0,  SPEED, 0);

	gameObject->getTransform()->setPosition(position);
}
