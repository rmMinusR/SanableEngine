#pragma once

#include <memory>
#include <optional>
#include <string>

#include "math/Vector2.inl"

class Application;
class WindowUserLogic;

struct WindowSettings
{
	//Required
	std::string name;
	Vector2<int> size;
	WindowUserLogic* userLogic = nullptr; //Owned by self, nulled on build, NOT nulled on destroy!

	//Optional
	std::optional<Vector2<int>> position;
	
	WindowSettings(const std::string& name, int width, int height);
	~WindowSettings();

	WindowSettings(WindowSettings&& mov);
	WindowSettings& operator=(WindowSettings&& mov);

	WindowSettings(const WindowSettings& cpy) = delete;
	WindowSettings& operator=(const WindowSettings& cpy) = delete;
};
