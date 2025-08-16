#include "WindowSettings.hpp"

#include <cassert>

WindowSettings::WindowSettings(const std::string& name, int width, int height) :
	name(name),
	size(width, height),
	userLogic(nullptr)
{
}

WindowSettings::~WindowSettings()
{
	assert(!userLogic && "Resource leak!");
}

WindowSettings::WindowSettings(WindowSettings&& mov)
{
	*this = std::move(mov);
}

WindowSettings& WindowSettings::operator=(WindowSettings&& mov)
{
	this->name = std::move(mov.name);
	this->size = std::move(mov.size);
	this->userLogic = std::move(mov.userLogic);
	this->position = std::move(mov.position);

	mov.userLogic = nullptr;

	return *this;
}
