#include "PlayerController.hpp"

#include "GameObject.hpp"

#include <SDL_keyboard.h>

PlayerController::PlayerController() :
	PlayerController(1)
{
}

PlayerController::PlayerController(float moveSpeed) :
	Component(),
	moveSpeed(moveSpeed)
{
}

void PlayerController::Update()
{
	Vector3<float> position = getGameObject()->getTransform()->getPosition();
	
	int sz;
	Uint8 const * keys = SDL_GetKeyboardState(&sz); //Managed by SDL, do not free
	
	//Move position
	if (keys[SDL_SCANCODE_A]) position += Vector3<float>(-moveSpeed, 0, 0);
	if (keys[SDL_SCANCODE_D]) position += Vector3<float>( moveSpeed, 0, 0);
	if (keys[SDL_SCANCODE_W]) position += Vector3<float>(0,  moveSpeed, 0);
	if (keys[SDL_SCANCODE_S]) position += Vector3<float>(0, -moveSpeed, 0);

	getGameObject()->getTransform()->setPosition(position);
}
