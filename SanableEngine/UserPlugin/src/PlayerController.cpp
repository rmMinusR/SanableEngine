#include "PlayerController.hpp"

#include "game/GameObject.hpp"

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
	Vector3<float> input;
	
	int sz;
	Uint8 const * keys = SDL_GetKeyboardState(&sz); //Managed by SDL, do not free
	
	//Move position
	if (keys[SDL_SCANCODE_A     ]) input += Vector3<float>(-moveSpeed, 0, 0);
	if (keys[SDL_SCANCODE_D     ]) input += Vector3<float>( moveSpeed, 0, 0);
	if (keys[SDL_SCANCODE_W     ]) input += Vector3<float>(0, 0, -moveSpeed);
	if (keys[SDL_SCANCODE_S     ]) input += Vector3<float>(0, 0,  moveSpeed);
	if (keys[SDL_SCANCODE_SPACE ]) input += Vector3<float>(0,  moveSpeed, 0);
	if (keys[SDL_SCANCODE_LSHIFT]) input += Vector3<float>(0, -moveSpeed, 0);

	Transform* t = getGameObject()->getTransform();
	t->setPosition(t->transformVector(input) + t->getPosition());
}
