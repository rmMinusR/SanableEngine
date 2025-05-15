#pragma once

#include "game/Component.hpp"
#include "Camera.hpp"

#undef main

class CameraComponent : public Component
{
private:
	Camera config;
	static CameraComponent* main; // TODO this should prob be on Game

public:
	ENGINECORE_API CameraComponent();
	ENGINECORE_API virtual ~CameraComponent();

	ENGINECORE_API static CameraComponent* getMain();
	ENGINECORE_API void setMain();

	ENGINECORE_API Camera* getConfig();
	ENGINECORE_API const Camera* getConfig() const;
};
