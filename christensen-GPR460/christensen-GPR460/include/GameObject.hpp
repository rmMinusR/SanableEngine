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
    ColliderColorChanger* CreateColliderColorChanger(SDL_Color normalColor, SDL_Color overlapColor);

    virtual void Update();
    virtual void Render();

    inline RectangleRenderer*    GetRenderer            () { return renderer    ; }
    inline RectangleCollider*    GetCollider            () { return collider    ; }
    inline PlayerController*     GetPlayerController    () { return player      ; }
    inline ColliderColorChanger* GetColliderColorChanger() { return colorChanger; }

protected:
    Transform transform;
    RectangleRenderer*    renderer;
    RectangleCollider*    collider;
    PlayerController*     player;
    ColliderColorChanger* colorChanger;

public:
    inline Transform* getTransform() { return &transform; }
};
