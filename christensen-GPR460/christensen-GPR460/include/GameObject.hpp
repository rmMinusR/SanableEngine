#pragma once

#include <SDL_pixels.h> //SDL_Color

#include "Transform.hpp"

class RectangleRenderer;
class RectangleCollider;
class PlayerController;
class ColliderColorChanger;

class GameObject
{
public:
    GameObject();
    GameObject(Transform&& initialTransform);
    ~GameObject();

    RectangleRenderer* CreateRenderer(float w, float h, SDL_Color color);
    RectangleCollider* CreateCollider(float w, float h);
    PlayerController* CreatePlayerController();
    ColliderColorChanger* CreateColliderColorChanger();

protected:
    Transform transform;
    RectangleRenderer* renderer;
    RectangleCollider* collider;
    PlayerController*  player;
    ColliderColorChanger* colorChanger;
};
 