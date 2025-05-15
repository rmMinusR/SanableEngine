#include "game/CameraComponent.hpp"

CameraComponent* CameraComponent::main = nullptr;

CameraComponent::CameraComponent()
{
	if (!main) main = this;
}

CameraComponent::~CameraComponent()
{
	if (main == this) main = nullptr;
}

CameraComponent* CameraComponent::getMain()
{
	return main;
}

void CameraComponent::setMain()
{
	main = this;
}

Camera* CameraComponent::getConfig()
{
	return &config;
}

const Camera* CameraComponent::getConfig() const
{
	return &config;
}
